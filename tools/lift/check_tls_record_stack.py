#!/usr/bin/env python3
"""Pin the two TLS record-stack bodies and the layout they imply.

Verifies, against the pinned image:
  * 0x12300 is 0x4d bytes and 0x12350 is 0x1f bytes, both FDE function starts;
  * the bodies contain the `%fs:0` load (`64 48 8b 04 25 00 00 00 00`), the link-time offset add
    through `0x15fff98`, the count load/store at `0x400`, and the `count << 5` stride;
  * pop is a `decl 0x400(%rax)`;
  * the constants in the header match those observations.
Skips cleanly when the private image is absent (module wiring is still checked).
"""

from __future__ import annotations

import hashlib
import importlib.util
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF = ROOT / "local/re/eboot.elf"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
HEADER = ROOT / "native/core/tls_record_stack.hpp"
SOURCE = ROOT / "native/core/tls_record_stack.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"

PUSH = (0x12300, 0x4D, "a540fb70abe2a28ad194e0096b98492e1d69d931831acee7d271c220244d00c8")
POP = (0x12350, 0x1F, "c2ac4f9f6cfa8e3272eaedaf5a50dda9eb487a96db105adf0425fb9cc2d7dbbe")
FS0 = bytes.fromhex("64488b042500000000")


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    header = HEADER.read_text()
    source = SOURCE.read_text()
    cmake = CMAKE.read_text()

    for needle in ("kTlsRecordCountOffset = 0x400", "kTlsRecordStride = 0x20",
                   "kTlsRecordCapacity = kTlsRecordCountOffset / kTlsRecordStride",
                   "kTlsRecordControlBlockOffsetCell = 0x15fff98", "capacity,"):
        require(needle in header, f"header missing {needle}")
    require("capacity" in source and "out_of_range" in source, "source lacks the status paths")
    for needle in ("core/tls_record_stack.cpp", "tests/test_tls_record_stack.cpp",
                   "test_tls_record_stack"):
        require(needle in cmake, f"cmake missing {needle}")

    if not ELF.is_file() or not FDE.is_file():
        print("SKIP check_tls_record_stack: private image not present (module wiring checked)")
        return 0

    elf = ELF.read_bytes()
    _, program_headers = d.program_headers(elf)
    image = d.Image(elf, program_headers)

    spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
    census = importlib.util.module_from_spec(spec)
    sys.modules["cfg_census"] = census
    spec.loader.exec_module(census)
    fdes, _duplicates = census.load_fde_ranges(FDE)

    for rva, size, sha in (PUSH, POP):
        body = image.read(rva, size)
        require(hashlib.sha256(body).hexdigest() == sha, f"0x{rva:x} body sha")
        found = fdes.find(rva)
        require(found is not None and found[0] == rva, f"0x{rva:x} is not an FDE function start")
        require(found[1] == size, f"0x{rva:x} FDE length {found[1]:#x} != {size:#x}")
        require(FS0 in body, f"0x{rva:x} lacks the fs:0 load")
        # The link-time offset cell is referenced through a rip-relative add.
        require(body.count(bytes.fromhex("480305")) == 1, f"0x{rva:x} offset add shape")

    push = image.read(*PUSH[:2])
    pop = image.read(*POP[:2])
    require(bytes.fromhex("8b8800040000") in push, "push lacks 'mov 0x400(%rax),%ecx'")
    require(bytes.fromhex("899000040000") in push, "push lacks 'mov %edx,0x400(%rax)'")
    require(bytes.fromhex("48c1e105") in push, "push lacks 'shl $0x5,%rcx' (the 0x20 stride)")
    require(bytes.fromhex("4c893408") in push, "push lacks 'mov %r14,(%rax,%rcx)'")
    require(bytes.fromhex("8b53108954 0818".replace(" ", "")) in push,
            "push lacks the 0x18 field store")
    require(bytes.fromhex("ff8800040000") in pop, "pop lacks 'decl 0x400(%rax)'")
    # Retail stores the incremented count before writing the record: count store precedes the record.
    require(push.find(bytes.fromhex("899000040000")) < push.find(bytes.fromhex("4c893408")),
            "push stores the count after the record, which contradicts the header comment")

    # The consumer idiom: count at +0x400, bail when zero, take the LAST record (count-1)*0x20.
    consumer_a = image.read(0x12007, 0x13)
    consumer_b = image.read(0x12890, 0x10)
    require(consumer_a.hex() == "8b880004000085c9740bffc948c1e1054801c8",
            "0x12007 consumer idiom changed")
    require(consumer_b.startswith(bytes.fromhex("8b880004000085c9740effc94489ea48")),
            "0x12890 consumer idiom changed")
    for site, body, sha in (
            (0x12007, consumer_a, "05b31f8c35c9c69c5c89e481aba07e78be3b0b7f4dcb566b337264a1da1135d1"),
            (0x12890, consumer_b, "c702003734723209abba7a550939e5858bd7037e0fe373377c76fe690429ff95")):
        require(hashlib.sha256(body).hexdigest() == sha, f"0x{site:x} consumer bytes changed")
        owner = fdes.find(site)
        require(owner is not None and owner[0] in (0x11FB0, 0x12820),
                f"0x{site:x} is no longer inside 0x11fb0/0x12820")
    require("tls_record_peek_top" in header, "header lacks the consumer accessor")
    require("empty," in header, "header lacks the empty status the consumer guard needs")

    # The link-time offset cell value is only meaningful at load time; record what is in the file.
    cell = struct.unpack("<q", image.read(0x15FFF98, 8))[0]
    print(f"0x15fff98 file value = {cell} (relocated at load time; not modelled)")

    print("OK check_tls_record_stack: both bodies, the 0x400 count, the 0x20 stride and the "
          "count-then-record order match the header")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
