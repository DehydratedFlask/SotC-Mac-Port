#!/usr/bin/env python3
"""ELF tie for the resolved status-code lookup 0xe21420 (vtable 0x15e2028 slot 0x2a0)."""

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
RVA = 0xE21420
SIZE = 0x2A9
END = 0xE216C9
BODY_SHA256 = "8346cf545fa3c478cc44a90392a1020e76d729424a0efe09ff429f679250f2b9"
VTABLE = 0x15E2028
SLOT = 0x2A0
RELOC = 0x15E22C8
STRCMP = 0xF8F490
HEADER = ROOT / "native/core/vtable_15e2028_slot2a0_e21420.hpp"
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
        ("415741564155415453", "saves r15/r14/r13/r12/rbx"),
        ("4883ec18", "0x18 frame"),
        ("4989f6", "ctx -> r14"),
        ("498b7e48", "provider load at ctx+0x48"),
        ("ff5028", "provider slot 0x28 call"),
        ("ff5018", "object slot 0x18 call"),
        ("ff5020", "object slot 0x20 call"),
        ("ff5078", "provider slot 0x78 call"),
        ("ff9088000000", "provider slot 0x88 call"),
        ("83f80e", "state compare 0x0e"),
        ("4883f801", "kind compare 1"),
        ("41bf13000000", "status 0x13"),
        ("41bf14000000", "status 0x14"),
        ("41bf1b000000", "status 0x1b"),
        ("41ffe0", "tail jmp through r8"),
    ]
    for needle, label in anchors:
        require(bytes.fromhex(needle) in body, f"missing {label}")

    # The strcmp call sits at body offset 0xb5; resolve it from the bytes.
    offset = 0xB5
    require(body[offset] == 0xE8, "missing strcmp call at 0xb5")
    target = RVA + offset + 5 + struct.unpack("<i", body[offset + 1:offset + 5])[0]
    require(target == STRCMP, f"strcmp target is {target:#x}")

    require(img.read(END, 4) == bytes.fromhex("90909090"), "padding after FDE end")

    slot_value = struct.unpack("<Q", img.read(VTABLE + SLOT, 8))[0]
    require(slot_value == RVA, "vtable slot does not point at 0xe21420")

    rows = [row for row in d.parse(elf)["relocs"] if row["target"] == RELOC]
    require(len(rows) == 1, f"expected one RELA row at {RELOC:#x}")
    require(rows[0]["type"] == 8 and rows[0]["addend"] == RVA, "RELA type/addend")

    header = HEADER.read_text()
    for needle in (
        "kVtable15e2028Slot2A0_e21420Rva = 0xe21420",
        "kVtable15e2028Slot2A0_e21420Size = 0x2a9",
        "kVtable15e2028Slot2A0_e21420VtableAddress = 0x15e2028",
        "kVtable15e2028Slot2A0_e21420SlotOffset = 0x2a0",
        "kVtable15e2028Slot2A0_e21420RelocAddress = 0x15e22c8",
        "kVtable15e2028Slot2A0_e21420ProviderOffset = 0x48",
        "kVtable15e2028Slot2A0_e21420ExpectedState = 0x0e",
        "kVtable15e2028Slot2A0_e21420StrcmpTarget = 0xf8f490",
    ):
        require(needle in header, f"header constant missing: {needle}")

    require("vtable_15e2028_slot2a0_e21420.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_15e2028_slot2a0" in CMAKE.read_text(), "cmake test")
    require("0xe21420" in json.loads(ENTRY_MAP.read_text())["entries"], "entry map missing 0xe21420")

    print("OK check_vtable_15e2028_slot2a0_e21420: guard ladder, strcmp, slot, RELA and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
