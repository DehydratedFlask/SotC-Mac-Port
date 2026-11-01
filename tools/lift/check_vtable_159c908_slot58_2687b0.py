#!/usr/bin/env python3
"""ELF tie for the resolved teardown target 0x2687b0 (vtable 0x159c908 slot 0x58)."""

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
RVA = 0x2687B0
SIZE = 0x1C
END = 0x2687CC
BODY = bytes.fromhex("554889e553504889fbe8120000004889df4883c4085b5de9e4e8daff")
BODY_SHA256 = "cd327f9b18614bdb73e3b9f47c0411da9111c4ab0e7ae34d6cf1244217645397"
VTABLE = 0x159C908
SLOT = 0x58
RELOC = 0x159C960
PRE_CALL = 0x2687D0
FREE = 0x170B0
HEADER = ROOT / "native/core/vtable_159c908_slot58_2687b0.hpp"
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
    require(body == BODY, "body bytes")

    # Structure, not only the hash: prologue, saved this, relative pre-call, reload, tail free.
    require(body[0:4] == bytes.fromhex("554889e5"), "missing frame prologue")
    require(body[6:9] == bytes.fromhex("4889fb"), "missing save of this")
    require(body[9:10] == bytes.fromhex("e8"), "missing pre-call")
    pre_rel = struct.unpack("<i", body[10:14])[0]
    require(RVA + 14 + pre_rel == PRE_CALL, "pre-call target")
    require(body[14:17] == bytes.fromhex("4889df"), "missing this reload")
    require(body[17:21] == bytes.fromhex("4883c408"), "missing stack unwind")
    require(body[22:23] == bytes.fromhex("5d"), "missing pop rbp")
    require(body[-5] == 0xE9, "missing tail jmp")
    tail_rel = struct.unpack("<i", body[-4:])[0]
    require(END + tail_rel == FREE, "tail jmp target")

    require(img.read(END, 4) == bytes.fromhex("90909090"), "padding after FDE end")

    slot_value = struct.unpack("<Q", img.read(VTABLE + SLOT, 8))[0]
    require(slot_value == RVA, "vtable slot does not point at 0x2687b0")

    rows = [row for row in d.parse(elf)["relocs"] if row["target"] == RELOC]
    require(len(rows) == 1, f"expected one RELA row at {RELOC:#x}")
    require(rows[0]["type"] == 8 and rows[0]["addend"] == RVA, "RELA type/addend")

    header = HEADER.read_text()
    for needle in (
        "kVtable159c908Slot58_2687b0Rva = 0x2687b0",
        "kVtable159c908Slot58_2687b0Size = 0x1c",
        "kVtable159c908Slot58_2687b0VtableAddress = 0x159c908",
        "kVtable159c908Slot58_2687b0SlotOffset = 0x58",
        "kVtable159c908Slot58_2687b0RelocAddress = 0x159c960",
        "kVtable159c908Slot58_2687b0PreCallRva = 0x2687d0",
        "kVtable159c908Slot58_2687b0FreeTargetRva = 0x170b0",
    ):
        require(needle in header, f"header constant missing: {needle}")

    require("vtable_159c908_slot58_2687b0.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_159c908_slot58" in CMAKE.read_text(), "cmake test")
    require("0x2687b0" in json.loads(ENTRY_MAP.read_text())["entries"], "entry map missing 0x2687b0")

    print("OK check_vtable_159c908_slot58_2687b0: body structure, slot, RELA and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
