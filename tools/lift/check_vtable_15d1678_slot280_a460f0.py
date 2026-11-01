#!/usr/bin/env python3
"""ELF tie for the resolved teardown target 0xa460f0 (vtable 0x15d1678 slot 0x280)."""

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
RVA = 0xA460F0
SIZE = 0x3E
END = 0xA4612E
BODY_SHA256 = "145fb280bb0b5f203bd2b8b23c1c03817df369ae8344ca984916a9bdb80c77c7"
VTABLE = 0x15D1678
SLOT = 0x280
RELOC = 0x15D18F8
HEADER = ROOT / "native/core/vtable_15d1678_slot280_a460f0.hpp"
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
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, "body sha")

    # Structural checks on the pinned bytes, not just the hash.
    require(body[0:4] == bytes.fromhex("554889e5"), "missing frame prologue")
    require(body[6:9] == bytes.fromhex("488d05"), "missing lea of the table base")
    displacement = struct.unpack("<i", body[9:13])[0]
    require(RVA + 13 + displacement == 0x15D1808, "table base lea target")
    require(body[27:30] == bytes.fromhex("488903"), "missing vtable store")
    require(body[37:40] == bytes.fromhex("ff5018"), "missing helper virtual call slot 0x18")
    require(body[40:43] == bytes.fromhex("4889df"), "missing this reload before 0x20b50")
    require(body[43:44] == bytes.fromhex("e8"), "missing relative call to 0x20b50")
    call_rel = struct.unpack("<i", body[44:48])[0]
    require(RVA + 48 + call_rel == 0x20B50, "post-call target")
    require(body[-5] == 0xE9, "missing tail jmp")
    tail_rel = struct.unpack("<i", body[-4:])[0]
    require(END + tail_rel == 0x170B0, "tail jmp target")

    require(img.read(END, 2) == bytes.fromhex("9090"), "padding after FDE end")

    slot_value = struct.unpack("<Q", img.read(VTABLE + SLOT, 8))[0]
    require(slot_value == RVA, "vtable slot does not point at 0xa460f0")

    rows = [row for row in d.parse(elf)["relocs"] if row["target"] == RELOC]
    require(len(rows) == 1, f"expected one RELA row at {RELOC:#x}")
    require(rows[0]["type"] == 8 and rows[0]["addend"] == RVA, "RELA type/addend")

    header = HEADER.read_text()
    for needle in (
        "kVtable15d1678Slot280_a460f0Rva = 0xa460f0",
        "kVtable15d1678Slot280_a460f0Size = 0x3e",
        "kVtable15d1678Slot280_a460f0VtableAddress = 0x15d1678",
        "kVtable15d1678Slot280_a460f0SlotOffset = 0x280",
        "kVtable15d1678Slot280_a460f0RelocAddress = 0x15d18f8",
        "kVtable15d1678Slot280_a460f0InstalledTable = 0x15d1818",
        "kVtable15d1678Slot280_a460f0PostCallRva = 0x20b50",
        "kVtable15d1678Slot280_a460f0FreeTargetRva = 0x170b0",
    ):
        require(needle in header, f"header constant missing: {needle}")

    require("vtable_15d1678_slot280_a460f0.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_15d1678_slot280" in CMAKE.read_text(), "cmake test")
    require("0xa460f0" in json.loads(ENTRY_MAP.read_text())["entries"], "entry map missing 0xa460f0")

    print("OK check_vtable_15d1678_slot280_a460f0: body structure, slot, RELA and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
