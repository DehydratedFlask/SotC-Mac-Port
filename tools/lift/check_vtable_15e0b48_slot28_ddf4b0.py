#!/usr/bin/env python3
"""ELF tie for the resolved manager-dispatch target 0xddf4b0 (vtable 0x15e0b48 slot 0x28)."""

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
RVA = 0xDDF4B0
SIZE = 0x34
END = 0xDDF4E4
BODY = bytes.fromhex(
    "554889e553504889fbe852feffff4885db741ae87804d5ff488b084889c74889de488b4918"
    "4883c4085b5dffe14883c4085b5dc3")
BODY_SHA256 = "a634359ff36f6e32be822366d733eb931b2c24fd86626992a6b75eb9c4fbe44e"
VTABLE = 0x15E0B48
SLOT = 0x28
RELOC = 0x15E0B70
PRE_CALL = 0xDDF310
MANAGER_FETCH = 0xB2F940
HEADER = ROOT / "native/core/vtable_15e0b48_slot28_ddf4b0.hpp"
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

    require(body[0:4] == bytes.fromhex("554889e5"), "missing prologue")
    require(body[6:9] == bytes.fromhex("4889fb"), "missing save of this")
    require(body[9:10] == bytes.fromhex("e8"), "missing pre-call")
    pre_rel = struct.unpack("<i", body[10:14])[0]
    require(RVA + 14 + pre_rel == PRE_CALL, "pre-call target")
    require(body[14:17] == bytes.fromhex("4885db"), "missing test rbx,rbx")
    require(body[17:19] == bytes.fromhex("741a"), "missing null early branch")
    require(body[19:20] == bytes.fromhex("e8"), "missing manager fetch call")
    fetch_rel = struct.unpack("<i", body[20:24])[0]
    require(RVA + 24 + fetch_rel == MANAGER_FETCH, "manager fetch target")
    require(body[24:27] == bytes.fromhex("488b08"), "missing vtable load")
    require(body[27:30] == bytes.fromhex("4889c7"), "missing manager arg")
    require(body[30:33] == bytes.fromhex("4889de"), "missing self arg")
    require(body[33:37] == bytes.fromhex("488b4918"), "missing slot 0x18 load")
    require(body[43:45] == bytes.fromhex("ffe1"), "missing indirect tail jmp")
    require(body[-1] == 0xC3, "null path does not end in retq")

    require(img.read(END, 4) == bytes.fromhex("90909090"), "padding after FDE end")

    slot_value = struct.unpack("<Q", img.read(VTABLE + SLOT, 8))[0]
    require(slot_value == RVA, "vtable slot does not point at 0xddf4b0")

    rows = [row for row in d.parse(elf)["relocs"] if row["target"] == RELOC]
    require(len(rows) == 1, f"expected one RELA row at {RELOC:#x}")
    require(rows[0]["type"] == 8 and rows[0]["addend"] == RVA, "RELA type/addend")

    header = HEADER.read_text()
    for needle in (
        "kVtable15e0b48Slot28_ddf4b0Rva = 0xddf4b0",
        "kVtable15e0b48Slot28_ddf4b0Size = 0x34",
        "kVtable15e0b48Slot28_ddf4b0VtableAddress = 0x15e0b48",
        "kVtable15e0b48Slot28_ddf4b0SlotOffset = 0x28",
        "kVtable15e0b48Slot28_ddf4b0RelocAddress = 0x15e0b70",
        "kVtable15e0b48Slot28_ddf4b0PreCallRva = 0xddf310",
        "kVtable15e0b48Slot28_ddf4b0ManagerFetchRva = 0xb2f940",
        "kVtable15e0b48Slot28_ddf4b0ManagerSlot = 0x18",
    ):
        require(needle in header, f"header constant missing: {needle}")

    require("vtable_15e0b48_slot28_ddf4b0.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_15e0b48_slot28" in CMAKE.read_text(), "cmake test")
    require("0xddf4b0" in json.loads(ENTRY_MAP.read_text())["entries"], "entry map missing 0xddf4b0")

    print("OK check_vtable_15e0b48_slot28_ddf4b0: body structure, slot, RELA and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
