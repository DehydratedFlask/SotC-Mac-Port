#!/usr/bin/env python3
"""ELF tie for the four low-RVA slots added by the vtable-closure sweep.

Each unit pins, from the pinned image: the body bytes and their sha256, the FDE length, the slot
qword at `vtable + slot`, the single type-8 dynamic relocation with a matching addend, and the
entry-map record. The CMake sources and test target are checked too.
"""

from __future__ import annotations

import hashlib
import importlib.util
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
FDE_JSON = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"

UNITS = [
    {
        "rva": 0x13D50,
        "size": 0x3,
        "body": "31c0c3",
        "sha": "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf",
        "vtable": 0x1594AE8,
        "slot": 0x40,
        "reloc": 0x1594B28,
        "header": ROOT / "native/core/vtable_1594ae8_slot40_13d50.hpp",
        "needles": [
            "kVtable1594ae8Slot40_13d50Rva = 0x13d50",
            "kVtable1594ae8Slot40_13d50SlotOffset = 0x40",
            "kVtable1594ae8Slot40_13d50RelocAddress = 0x1594b28",
        ],
    },
    {
        "rva": 0x13D60,
        "size": 0x9,
        "body": "488d476048894770c3",
        "sha": "cef6fb4a6a0f10ff5172d5854fd0cf2642011e2405fdcde020dbb18c01cc469f",
        "vtable": 0x1594AE8,
        "slot": 0x50,
        "reloc": 0x1594B38,
        "header": ROOT / "native/core/vtable_1594ae8_slot50_13d60.hpp",
        "needles": [
            "kVtable1594ae8Slot50_13d60Rva = 0x13d60",
            "kVtable1594ae8Slot50_13d60SlotOffset = 0x50",
            "kVtable1594ae8Slot50_13d60RelocAddress = 0x1594b38",
        ],
    },
    {
        "rva": 0xC2C0,
        "size": 0x14,
        "body": "488d05b97f580148894720488b461048894710c3",
        "sha": "047e110aeff5fe3000666c334afa7c4e6c799a779125a7608c000e1eed1a7474",
        "vtable": 0x15949A0,
        "slot": 0x58,
        "reloc": 0x15949F8,
        "header": ROOT / "native/core/vtable_15949a0_slot58_c2c0.hpp",
        "needles": [
            "kVtable15949a0Slot58_c2c0Rva = 0xc2c0",
            "kVtable15949a0Slot58_c2c0SlotOffset = 0x58",
            "kVtable15949a0Slot58_c2c0RelocAddress = 0x15949f8",
        ],
    },
    {
        "rva": 0x30E50,
        "size": 0x5,
        "body": "e94b040000",
        "sha": "880f27689071e3923cb37b9b6959946f656b922cc8fd4331b4624509fb223649",
        "vtable": 0x1595320,
        "slot": 0x10,
        "reloc": 0x1595330,
        "header": ROOT / "native/core/vtable_1595320_slot10_30e50.hpp",
        "needles": [
            "kVtable1595320Slot10_30e50Rva = 0x30e50",
            "kVtable1595320Slot10_30e50SlotOffset = 0x10",
            "kVtable1595320Slot10_30e50RelocAddress = 0x1595330",
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

    fdes = None
    if FDE_JSON.is_file():
        spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
        census = importlib.util.module_from_spec(spec)
        sys.modules["cfg_census"] = census
        spec.loader.exec_module(census)
        fdes, _duplicates = census.load_fde_ranges(FDE_JSON)

    for unit in UNITS:
        rva = unit["rva"]
        body = img.read(rva, unit["size"])
        require(hashlib.sha256(body).hexdigest() == unit["sha"], f"{rva:#x} body sha")
        require(body == bytes.fromhex(unit["body"]), f"{rva:#x} body bytes")

        slot_value = struct.unpack("<Q", img.read(unit["vtable"] + unit["slot"], 8))[0]
        require(slot_value == rva, f"{rva:#x} slot qword is {slot_value:#x}")
        rows = [row for row in relocs if row["target"] == unit["reloc"]]
        require(len(rows) == 1, f"{rva:#x} expects one RELA row at {unit['reloc']:#x}")
        require(rows[0]["type"] == 8, f"{rva:#x} expects a type-8 relocation")
        require(rows[0]["addend"] == rva, f"{rva:#x} RELA addend")

        if fdes is not None:
            found = fdes.find(rva)
            require(found is not None and found[0] == rva, f"{rva:#x} is not an FDE start")
            require(found[1] == unit["size"], f"{rva:#x} FDE length {found[1]:#x}")

        header = (ROOT / unit["header"]).read_text()
        for needle in unit["needles"]:
            require(needle in header, f"{rva:#x} header constant missing: {needle}")
        require(f"0x{rva:x}" in entry_map, f"{rva:#x} missing from entry map")

    for needle in (
        'core/vtable_1594ae8_slot40_13d50.cpp',
        'core/vtable_1594ae8_slot50_13d60.cpp',
        'core/vtable_15949a0_slot58_c2c0.cpp',
        'core/vtable_1595320_slot10_30e50.cpp',
        'test_vtable_siblings_low',
    ):
        require(needle in cmake, f"cmake missing {needle}")

    print(f"OK check_vtable_siblings_low: {len(UNITS)} bodies, slots, RELA rows and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
