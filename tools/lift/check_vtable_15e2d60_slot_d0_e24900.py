#!/usr/bin/env python3
"""ELF tie for the resolved sub-dispatch target 0xe24900 (vtable 0x15e2d60 slot 0xd0)."""

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
RVA = 0xE24900
SIZE = 0x84
END = 0xE24984
BODY_SHA256 = "4bfba6b71177ce67f282784f0202758527ed91e8c6178a72494c74d555f191f9"
VTABLE = 0x15E2D60
SLOT = 0xD0
RELOC = 0x15E2E30
HEADER = ROOT / "native/core/vtable_15e2d60_slot_d0_e24900.hpp"
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

    anchors = [
        ("554889e5", "prologue"),
        ("415741564155415453", "saves five registers"),
        ("4889fb", "this -> rbx"),
        ("4989f6", "arg1 -> r14"),
        ("4c8b7b08", "sub load at this+0x8"),
        ("4c8b6940", "sub slot 0x40 load"),
        ("ff5028", "this slot 0x28 call"),
        ("ff5048", "this slot 0x48 call"),
        ("41ffd5", "call through r13"),
        ("b81b000000", "status 0x1b"),
        ("49890e", "store into *arg1"),
        ("4885c9", "null result test"),
        ("ff90b8000000", "result slot 0xb8 call"),
        ("ff5010", "release slot 0x10 call"),
        ("49c70600000000", "clear *arg1"),
        ("4883c4085b415c415d415e415f5dc3", "epilogue"),
    ]
    for needle, label in anchors:
        require(bytes.fromhex(needle) in body, f"missing {label}")

    require(img.read(END, 4) == bytes.fromhex("90909090"), "padding after FDE end")

    slot_value = struct.unpack("<Q", img.read(VTABLE + SLOT, 8))[0]
    require(slot_value == RVA, "vtable slot does not point at 0xe24900")

    rows = [row for row in d.parse(elf)["relocs"] if row["target"] == RELOC]
    require(len(rows) == 1, f"expected one RELA row at {RELOC:#x}")
    require(rows[0]["type"] == 8 and rows[0]["addend"] == RVA, "RELA type/addend")

    header = HEADER.read_text()
    for needle in (
        "kVtable15e2d60SlotD0_e24900Rva = 0xe24900",
        "kVtable15e2d60SlotD0_e24900Size = 0x84",
        "kVtable15e2d60SlotD0_e24900VtableAddress = 0x15e2d60",
        "kVtable15e2d60SlotD0_e24900SlotOffset = 0xd0",
        "kVtable15e2d60SlotD0_e24900RelocAddress = 0x15e2e30",
        "kVtable15e2d60SlotD0_e24900SubOffset = 0x08",
        "kVtable15e2d60SlotD0_e24900StatusNull = 0x1b",
    ):
        require(needle in header, f"header constant missing: {needle}")

    require("vtable_15e2d60_slot_d0_e24900.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_15e2d60_slot_d0" in CMAKE.read_text(), "cmake test")
    require("0xe24900" in json.loads(ENTRY_MAP.read_text())["entries"], "entry map missing 0xe24900")

    print("OK check_vtable_15e2d60_slot_d0_e24900: sub dispatch, status paths, slot, RELA and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
