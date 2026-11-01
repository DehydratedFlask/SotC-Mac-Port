#!/usr/bin/env python3
"""ELF tie for the resolved status-gated helper target 0xe26f90 (vtable 0x15ed2a0 slot 0xc0)."""

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
RVA = 0xE26F90
SIZE = 0xB7
END = 0xE27047
BODY_SHA256 = "d36ac3fdd87ccccbb0320c50c60c089d395f893a71dd342d2619e16cacd0ad4d"
VTABLE = 0x15ED2A0
SLOT = 0xC0
RELOC = 0x15ED360
NAME = 0x131706D
HELPER = 0xE255B0
HEADER = ROOT / "native/core/vtable_15ed2a0_slot_c0_e26f90.hpp"
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

    for needle, label in (
        ("554889e5", "prologue"),
        ("41574156415453", "saves five registers"),
        ("4883ec70", "0x70 frame"),
        ("4c8b25", "canary load"),
        ("ff5078", "slot 0x78 call"),
        ("ff9090000000", "slot 0x90 call"),
        ("85c0", "status test"),
        ("740431c0", "early zero return"),
        ("4883c4705b415c415e415f5dc3", "epilogue"),
    ):
        require(bytes.fromhex(needle) in body, f"missing {label}")

    lea_at = 0x55
    require(body[lea_at:lea_at + 2] == bytes.fromhex("488d"), "missing name lea")
    name_target = RVA + lea_at + 7 + struct.unpack("<i", body[lea_at + 3:lea_at + 7])[0]
    require(name_target == NAME, f"name lea target {name_target:#x}")

    call_at = 0x95
    require(body[call_at] == 0xE8, "missing helper call")
    helper_target = RVA + call_at + 5 + struct.unpack("<i", body[call_at + 1:call_at + 5])[0]
    require(helper_target == HELPER, f"helper target {helper_target:#x}")

    require(img.read(END, 4) == bytes.fromhex("90909090"), "padding after FDE end")

    slot_value = struct.unpack("<Q", img.read(VTABLE + SLOT, 8))[0]
    require(slot_value == RVA, "vtable slot does not point at 0xe26f90")

    rows = [row for row in d.parse(elf)["relocs"] if row["target"] == RELOC]
    require(len(rows) == 1, f"expected one RELA row at {RELOC:#x}")
    require(rows[0]["type"] == 8 and rows[0]["addend"] == RVA, "RELA type/addend")

    header = HEADER.read_text()
    for needle in (
        "kVtable15ed2a0SlotC0_e26f90Rva = 0xe26f90",
        "kVtable15ed2a0SlotC0_e26f90Size = 0xb7",
        "kVtable15ed2a0SlotC0_e26f90VtableAddress = 0x15ed2a0",
        "kVtable15ed2a0SlotC0_e26f90SlotOffset = 0xc0",
        "kVtable15ed2a0SlotC0_e26f90RelocAddress = 0x15ed360",
        "kVtable15ed2a0SlotC0_e26f90NameRva = 0x131706d",
        "kVtable15ed2a0SlotC0_e26f90HelperRva = 0xe255b0",
    ):
        require(needle in header, f"header constant missing: {needle}")

    require("vtable_15ed2a0_slot_c0_e26f90.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_15ed2a0_slot_c0" in CMAKE.read_text(), "cmake test")
    require("0xe26f90" in json.loads(ENTRY_MAP.read_text())["entries"], "entry map missing 0xe26f90")

    print("OK check_vtable_15ed2a0_slot_c0_e26f90: status gate, helper call, slot, RELA and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
