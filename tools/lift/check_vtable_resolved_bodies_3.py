#!/usr/bin/env python3
"""ELF tie for the two smallest newly resolved targets: 0xc2e0 and 0xa40e20."""

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
ENTRY_MAP = ROOT / "docs/entry_map.json"
CMAKE = ROOT / "native/CMakeLists.txt"

UNITS = [
    {
        "rva": 0xC2E0,
        "size": 0x9,
        "body": bytes.fromhex("488b461048894710c3"),
        "sha": "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd",
        "vtable": 0x15D1678,
        "slot": 0xE8,
        "reloc": 0x15D1760,
        "header": ROOT / "native/core/vtable_15d1678_slot_e8_c2e0.hpp",
        "needles": [
            "kVtable15d1678SlotE8_c2e0Rva = 0xc2e0",
            "kVtable15d1678SlotE8_c2e0SlotOffset = 0xe8",
            "kVtable15d1678SlotE8_c2e0RelocAddress = 0x15d1760",
        ],
    },
    {
        "rva": 0xA40E20,
        "size": 0x1,
        "body": bytes.fromhex("c3"),
        "sha": "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e",
        "vtable": 0x15D1678,
        "slot": 0x68,
        "reloc": 0x15D16E0,
        "header": ROOT / "native/core/vtable_15d1678_slot_68_a40e20.hpp",
        "needles": [
            "kVtable15d1678Slot68_a40e20Rva = 0xa40e20",
            "kVtable15d1678Slot68_a40e20SlotOffset = 0x68",
            "kVtable15d1678Slot68_a40e20RelocAddress = 0x15d16e0",
        ],
    },
]


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)
    relocs = d.parse(elf)["relocs"]
    entry_map = json.loads(ENTRY_MAP.read_text())["entries"]
    cmake = CMAKE.read_text()

    for unit in UNITS:
        body = img.read(unit["rva"], unit["size"])
        require(hashlib.sha256(body).hexdigest() == unit["sha"], f"{unit['rva']:#x} body sha")
        require(body == unit["body"], f"{unit['rva']:#x} body bytes")

        slot_value = struct.unpack("<Q", img.read(unit["vtable"] + unit["slot"], 8))[0]
        require(slot_value == unit["rva"], f"{unit['rva']:#x} slot qword")

        rows = [row for row in relocs if row["target"] == unit["reloc"]]
        require(len(rows) == 1, f"{unit['rva']:#x} expects one RELA row at {unit['reloc']:#x}")
        require(rows[0]["type"] == 8, f"{unit['rva']:#x} expects a type-8 relocation")
        require(rows[0]["addend"] == unit["rva"], f"{unit['rva']:#x} RELA addend")

        header = unit["header"].read_text()
        for needle in unit["needles"]:
            require(needle in header, f"{unit['rva']:#x} header constant missing: {needle}")
        require(f"0x{unit['rva']:x}" in entry_map, f"{unit['rva']:#x} missing from entry map")

    for needle in ("vtable_15d1678_slot_e8_c2e0.cpp", "vtable_15d1678_slot_68_a40e20.cpp",
                   "test_vtable_resolved_bodies_3"):
        require(needle in cmake, f"cmake missing {needle}")

    print("OK check_vtable_resolved_bodies_3: 2 bodies, slots, RELA rows and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
