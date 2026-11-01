#!/usr/bin/env python3
"""ELF tie for the second resolved vtable slot: 0x15949a0 + 0x88 -> 0x187c0."""

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
RVA = 0x187C0
SIZE = 0x1C
END = 0x187DC
BYTES = bytes.fromhex("554889e553504889fbe852ee02004889df4883c4085b5de9d4e8ffff")
BYTES_SHA256 = "e0daf4712d19799ca3950703ceece991e17bf018a79fe04b7713edb3ce5c8cde"
VTABLE = 0x15949A0
SLOT = 0x88
RELOC = 0x1594A28
HEADER = ROOT / "native/core/vtable_15949a0_slot88_187c0.hpp"
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
    require(hashlib.sha256(body).hexdigest() == BYTES_SHA256, "body sha")
    require(body == BYTES, "body bytes")
    require(img.read(END, 4) == bytes.fromhex("90909090"), "padding after FDE end")

    # The body must end in the tag-1 free tail jump: e9 rel32 -> 0x170b0.
    require(body[-5] == 0xE9, "missing tail jmp")
    rel = struct.unpack("<i", body[-4:])[0]
    require(END + rel == 0x170B0, "tail jmp target is not 0x170b0")

    slot_value = struct.unpack("<Q", img.read(VTABLE + SLOT, 8))[0]
    require(slot_value == RVA, "vtable slot does not point at 0x187c0")

    info = d.parse(elf)
    rows = [row for row in info["relocs"] if row["target"] == RELOC]
    require(len(rows) == 1, f"expected one RELA row at {RELOC:#x}, found {len(rows)}")
    require(rows[0]["addend"] == RVA, "rela addend is not 0x187c0")

    header = HEADER.read_text()
    for needle in (
        "kVtable15949a0Slot88_187c0Rva = 0x187c0",
        "kVtable15949a0Slot88_187c0Size = 0x1c",
        "kVtable15949a0Slot88_187c0VtableAddress = 0x15949a0",
        "kVtable15949a0Slot88_187c0SlotOffset = 0x88",
        "kVtable15949a0Slot88_187c0RelocAddress = 0x1594a28",
        "kVtable15949a0Slot88_187c0PreCallRva = 0x47620",
        "kVtable15949a0Slot88_187c0FreeTargetRva = 0x170b0",
    ):
        require(needle in header, f"header constant missing: {needle}")

    require("vtable_15949a0_slot88_187c0.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_15949a0_slot88" in CMAKE.read_text(), "cmake test")

    mapping = json.loads(ENTRY_MAP.read_text())["entries"]
    require("0x187c0" in mapping, "entry map is stale; rerun tools/lift/gen_entry_map.py")

    print("OK check_vtable_15949a0_slot88_187c0: body, tail jmp, vtable, RELA and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
