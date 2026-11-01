#!/usr/bin/env python3
"""ELF tie for the resolved dual-vcall target 0xe16a00 (vtable 0x15f2f08 slot 0x38)."""

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
RVA = 0xE16A00
SIZE = 0x26
END = 0xE16A26
BODY = bytes.fromhex(
    "554889e54156534889fb4989f6488b03ff5018488b034889df4c89f6488b40205b415e5dffe0")
BODY_SHA256 = "351ea6e0441c875dddf92c0d8ad08fbbffb5aacae4457a9412fffde7d0143c11"
VTABLE = 0x15F2F08
SLOT = 0x38
RELOC = 0x15F2F40
HEADER = ROOT / "native/core/vtable_15f2f08_slot38_e16a00.hpp"
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
    require(body[7:10] == bytes.fromhex("4889fb"), "missing save of this")
    require(body[10:13] == bytes.fromhex("4989f6"), "missing save of arg1")
    require(body[13:16] == bytes.fromhex("488b03"), "missing first vtable load")
    require(body[16:19] == bytes.fromhex("ff5018"), "missing slot 0x18 call")
    require(body[19:22] == bytes.fromhex("488b03"), "missing vtable reload")
    require(body[22:25] == bytes.fromhex("4889df"), "missing this reload")
    require(body[25:28] == bytes.fromhex("4c89f6"), "missing arg1 reload")
    require(body[28:32] == bytes.fromhex("488b4020"), "missing slot 0x20 load")
    require(body[32:35] == bytes.fromhex("5b415e"), "missing register restores")
    require(body[35:36] == bytes.fromhex("5d"), "missing pop rbp")
    require(body[36:38] == bytes.fromhex("ffe0"), "missing indirect tail jmp")

    require(img.read(END, 4) == bytes.fromhex("90909090"), "padding after FDE end")

    slot_value = struct.unpack("<Q", img.read(VTABLE + SLOT, 8))[0]
    require(slot_value == RVA, "vtable slot does not point at 0xe16a00")

    rows = [row for row in d.parse(elf)["relocs"] if row["target"] == RELOC]
    require(len(rows) == 1, f"expected one RELA row at {RELOC:#x}")
    require(rows[0]["type"] == 8 and rows[0]["addend"] == RVA, "RELA type/addend")

    header = HEADER.read_text()
    for needle in (
        "kVtable15f2f08Slot38_e16a00Rva = 0xe16a00",
        "kVtable15f2f08Slot38_e16a00Size = 0x26",
        "kVtable15f2f08Slot38_e16a00VtableAddress = 0x15f2f08",
        "kVtable15f2f08Slot38_e16a00SlotOffset = 0x38",
        "kVtable15f2f08Slot38_e16a00RelocAddress = 0x15f2f40",
        "kVtable15f2f08Slot38_e16a00FirstCallSlot = 0x18",
        "kVtable15f2f08Slot38_e16a00TailCallSlot = 0x20",
    ):
        require(needle in header, f"header constant missing: {needle}")

    require("vtable_15f2f08_slot38_e16a00.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_15f2f08_slot38" in CMAKE.read_text(), "cmake test")
    require("0xe16a00" in json.loads(ENTRY_MAP.read_text())["entries"], "entry map missing 0xe16a00")

    print("OK check_vtable_15f2f08_slot38_e16a00: body structure, slot, RELA and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
