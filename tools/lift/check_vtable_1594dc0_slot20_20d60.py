#!/usr/bin/env python3
"""ELF tie for the one statically resolved vtable slot: 0x1594dc0 + 0x20 -> 0x20d60.

Pins the slot bytes, the file-backed vtable entry, the relocation row and the native constants.
"""

from __future__ import annotations

import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
RVA = 0x20D60
SIZE = 0x1A
END = 0x20D7A
BYTES = bytes.fromhex("807f18007401c3488d055abedd02c6471801488b30e906000000")
BYTES_SHA256 = "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"
VTABLE = 0x1594DC0
SLOT = 0x20
RELOC = 0x1594DE0
HEADER = ROOT / "native/core/vtable_1594dc0_slot20_20d60.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"
ENTRY_MAP = ROOT / "docs/entry_map.json"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(RVA, SIZE)
    require(hashlib.sha256(body).hexdigest() == BYTES_SHA256, "slot body sha")
    require(body == BYTES, "slot body bytes")
    require(img.read(END, 6) == bytes.fromhex("909090909090"), "tail padding after FDE end")

    slot_value = struct.unpack("<Q", img.read(VTABLE + SLOT, 8))[0]
    require(slot_value == RVA, "vtable slot does not point at 0x20d60")

    info = d.parse(elf)
    rows = [row for row in info["relocs"] if row["target"] == RELOC]
    require(len(rows) == 1, f"expected one RELA row at {RELOC:#x}, found {len(rows)}")
    require(rows[0]["addend"] == RVA, "rela addend is not 0x20d60")

    header = HEADER.read_text()
    for needle in (
        "kVtable1594dc0Slot20_20d60Rva = 0x20d60",
        "kVtable1594dc0Slot20_20d60Size = 0x1a",
        "kVtable1594dc0Slot20_20d60VtableAddress = 0x1594dc0",
        "kVtable1594dc0Slot20_20d60SlotOffset = 0x20",
        "kVtable1594dc0Slot20_20d60RelocAddress = 0x1594de0",
        "kVtable1594dc0Slot20_20d60GlobalAddress = 0x2dfcbc8",
        "kVtable1594dc0Slot20_20d60TailTargetRva = 0x20d80",
    ):
        require(needle in header, f"header constant missing: {needle}")

    require("vtable_1594dc0_slot20_20d60.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_1594dc0_slot20" in CMAKE.read_text(), "cmake test")

    mapping = json.loads(ENTRY_MAP.read_text())["entries"]
    require("0x20d60" in mapping, "entry map is stale; rerun tools/lift/gen_entry_map.py")

    print("OK check_vtable_1594dc0_slot20_20d60: body, vtable, RELA and entry map all agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
