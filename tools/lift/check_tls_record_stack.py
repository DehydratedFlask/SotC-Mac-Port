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
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF = ROOT / "local/re/eboot.elf"
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"
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


INSTRUCTION = re.compile(r"^\s*([0-9a-f]+):\s+[a-z][a-z0-9.]*\s+.*$")


def rows_by_address(start: int, size: int) -> list[str]:
    """Raw disassembly text for instruction lines in [start, start+size)."""
    texts = []
    for line in DIS.open(encoding="utf-8", errors="replace"):
        match = INSTRUCTION.match(line)
        if match is None:
            continue
        address = int(match.group(1), 16)
        if start <= address < start + size:
            texts.append(line.rstrip())
    return texts


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

    if not ELF.is_file() or not FDE.is_file() or not DIS.is_file():
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
    require("tls_alloc_site_push" in header and "tls_alloc_site_peek_top" in header,
            "header lacks the allocation-site helpers")
    require("empty," in header, "header lacks the empty status the consumer guard needs")

    # The push sites build {function, file, line} and push immediately before an allocation.
    def string_at(address: int, limit: int = 200) -> str:
        out = bytearray()
        for index in range(limit):
            try:
                byte = image.read(address + index, 1)[0]
            except Exception:
                break
            if byte == 0:
                break
            out.append(byte)
        return out.decode("utf-8", "replace")

    SITES = (
        # (push site, function-string address, file-string address, line immediate)
        (0x904EE, 0xF9F05C, 0xF9EF3B, 0x25D),
        (0x16EA05, 0xFABDDE, 0xFABD92, 0x2E),
        (0x91926, None, None, 0xE9),
    )
    checked = 0
    for site, function_address, file_address, line in SITES:
        # The push must be followed by an allocation call within 0x1c bytes.
        targets = {int(m.group(1), 16)
                   for text in rows_by_address(site, 0x1c)
                   for m in re.finditer(r"callq\s+0x([0-9a-f]+)", text)}
        require(targets & {0x17070, 0x12820},
                f"0x{site:x} is not followed by an allocation call (saw {sorted(hex(t) for t in targets)})")
        if file_address is None:
            continue
        function_text = string_at(function_address)
        file_text = string_at(file_address)
        require(function_text.strip(), f"0x{site:x} function string is empty")
        require(file_text.endswith(".cpp"), f"0x{site:x} file string is not a source path: {file_text!r}")
        require("\\" in file_text or "/" in file_text, f"0x{site:x} file string has no path separator")
        checked += 1
    require(checked == 2, "expected two fully decoded push sites")

    # The link-time offset cell value is only meaningful at load time; record what is in the file.
    cell = struct.unpack("<q", image.read(0x15FFF98, 8))[0]
    print(f"0x15fff98 file value = {cell} (relocated at load time; not modelled)")

    print("OK check_tls_record_stack: both bodies, the 0x400 count, the 0x20 stride and the "
          "count-then-record order match the header")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
