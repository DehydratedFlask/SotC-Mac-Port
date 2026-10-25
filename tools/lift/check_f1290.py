#!/usr/bin/env python3
"""ELF-tie the native lift of original function RVA 0x1290 (Init 0x531d60).

Pins the 195 B body, its block/extent boundaries, the three direct-call targets, the
single indirect call encoding, the resolvable slot-3 target, and the native module's
declared constants. Private: needs local/re/eboot.elf.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
RVA = 0x1290
SIZE = 195
FN_SHA256 = "185f03d382259a5e59452be06fbe87d67cd4b2e6a6bdb1bab85be95a3cf553fb"
END = 0x1353
NEXT_UNIT = 0x1360
CALLS = {0x12F5: 0x1BDE30, 0x1334: 0x21CD0, 0x134E: 0xF8F3A0}
VCALL_RVA = 0x1301
TABLE = 0x1594060
TABLE_SLOT3 = 0x19F0
NATIVE = ROOT / "native/core/init_1290.hpp"
IMPL = ROOT / "native/core/init_1290.cpp"
TEST = ROOT / "native/tests/test_init_1290.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    fn = img.read(RVA, SIZE)
    require(hashlib.sha256(fn).hexdigest() == FN_SHA256, "fn 0x1290 sha256")
    require(img.read(0x1284, 12) == b"\x90" * 12, "12 B nop pad before")
    require(img.read(END, 13) == b"\x90" * 13, "13 B nop pad after")
    require(img.read(0x134D, 1) == b"\xc3", "ret at 0x134d")

    for site, target in CALLS.items():
        op = img.read(site, 5)
        require(op[0] == 0xE8, f"call opcode at {site:#x}")
        require(site + 5 + struct.unpack("<i", op[1:])[0] == target, f"call target {site:#x}")

    require(img.read(VCALL_RVA, 3) == bytes.fromhex("ff5018"), "call *0x18(%rax)")
    require(img.read(0x129E, 3) == bytes.fromhex("4c8b35"), "guard slot load start")
    slot3 = struct.unpack("<Q", img.read(TABLE + 0x18, 8))[0]
    require(slot3 == TABLE_SLOT3, "table slot 3 = 0x19f0")
    require(img.read(TABLE_SLOT3, 1) == b"\xc3", "0x19f0 is a bare ret")
    require(img.read(NEXT_UNIT, 1) != b"\x90", "next unit starts at 0x1360")

    header = NATIVE.read_text()
    for token in (
        "kFn1290Rva = 0x1290",
        "kFn1290Size = 195",
        "kFn1290EndRva = 0x1353",
        "kFn1290RegAInvoke = 0x1a00",
        "kFn1290RegBInvoke = 0x1a10",
        "kFn1290RegAFn = 0xb10",
        "kFn1290RegBFn = 0xbb0",
        "kFn1290Table = 0x1594060",
        "NOT modelled",
        "0x15fe518",
    ):
        require(token in header, f"header token {token!r}")
    require("insert_1bde30" in header and "insert_21cd0" in header, "hook names")
    require("vcall_1594060_slot3" in header, "vcall hook")
    require("container_a" in header and "insert_1beeb0" in header, "real insert path")
    require(len(IMPL.read_text()) > 0, "impl present")
    require("0x20" in TEST.read_text() or "0x20" in header, "0x20 entry width documented")

    print(
        "OK check_f1290: extent=0x1290+195B sha=" + FN_SHA256[:12] + "…; "
        "calls 0x1bde30/0x21cd0/0xf8f3a0 pinned; vcall *0x18(%rax) at 0x1301; "
        "table 0x1594060 slot3=0x19f0 (bare ret); guard read-only; "
        "bodies/owners/guard-fail-path NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
