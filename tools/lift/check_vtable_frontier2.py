#!/usr/bin/env python3
"""ELF tie for the four targets added by the provenance-audited vtable-closure slice.

Each unit pins, from the pinned image: the body bytes and their sha256, the FDE length, the slot
qword at `table + slot`, the single type-8 dynamic relocation with a matching addend, and the
header constants. The script also checks the CMake sources and the entry map.

Entry map: `docs/entry_map.json` is generated from the native headers by
`tools/lift/gen_entry_map.py`; every target must be present and the record must point at the unit
header or at `vtable_resolved_entries.hpp`.
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
TABLE_HEADER = ROOT / "native/core/vtable_resolved_entries.hpp"

UNITS = [
    {
        "rva": 0x9B7B80,
        "size": 0x6,
        "body": "c5fa104738c3",
        "sha": "4d18a1ad4af5841716e72115fabfe97bb4e4b0d158ff1df19da4ee6972332ea8",
        "table": 0x15CF248,
        "slot": 0x68,
        "reloc": 0x15CF2B0,
        "header": ROOT / "native/core/vtable_15cf248_slot68_9b7b80.hpp",
        "needles": [
            "kVtable15cf248Slot68_9b7b80Rva = 0x9b7b80",
            "kVtable15cf248Slot68_9b7b80Size = 0x6",
            "kVtable15cf248Slot68_9b7b80VtableAddress = 0x15cf248",
            "kVtable15cf248Slot68_9b7b80SlotOffset = 0x68",
            "kVtable15cf248Slot68_9b7b80RelocAddress = 0x15cf2b0",
            "kVtable15cf248Slot68_9b7b80ValueOffset = 0x38",
        ],
        "sites": [0x21A695, 0x21BA92, 0x21BB84, 0x21C573, 0x21C725, 0x21D134, 0x21D306,
                  0x2F54DA, 0x3E4AF3, 0x3E4DC4, 0x3E7237, 0x3EDF1A, 0x3EE19F],
    },
    {
        "rva": 0xD7C4B0,
        "size": 0x3,
        "body": "31c0c3",
        "sha": "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf",
        "table": 0x15DE228,
        "slot": 0x20,
        "reloc": 0x15DE248,
        "header": ROOT / "native/core/vtable_15de228_slot20_d7c4b0.hpp",
        "needles": [
            "kVtable15de228Slot20_d7c4b0Rva = 0xd7c4b0",
            "kVtable15de228Slot20_d7c4b0Size = 0x3",
            "kVtable15de228Slot20_d7c4b0VtableAddress = 0x15de228",
            "kVtable15de228Slot20_d7c4b0SlotOffset = 0x20",
            "kVtable15de228Slot20_d7c4b0RelocAddress = 0x15de248",
        ],
        "sites": [0xBDA7E5],
    },
    {
        "rva": 0x6E92C0,
        "size": 0xD8,
        "body": (
            "554889e541574156534883ec584c8b3d4452f100488d35ed6edd01c5f810050ddcb8004c8d3"
            "556cef1004889f8488d7da84c8d4590498b0f48894de0c5f8114590488b0e4889c68b51104c"
            "89f1e8fec6f0ff488d055f984c01488b00488b58184885db74594889dfe8937cf7ff488b8860"
            "010000418b16488d75a84889df8911418b5604895104418b5608895108418b560c89510c488d"
            "0d743c8602488b80500100008b1189108b51048950048b51088950088b490c89480ce8527df7"
            "ff498b07483b45e0750b4883c4585b415e415f5dc3e809608a0090"
        ),
        "sha": "01e41774c5057a8405fb994b057b6f78737f70c32656d10a253c971ed5822c63",
        "table": 0x2DCB170,
        "slot": 0x08,
        "reloc": 0x2DCB178,
        "header": ROOT / "native/core/vtable_2dcb170_slot08_6e92c0.hpp",
        "needles": [
            "kVtable2dcb170Slot08_6e92c0Rva = 0x6e92c0",
            "kVtable2dcb170Slot08_6e92c0Size = 0xd8",
            "kVtable2dcb170Slot08_6e92c0TableAddress = 0x2dcb170",
            "kVtable2dcb170Slot08_6e92c0SlotOffset = 0x08",
            "kVtable2dcb170Slot08_6e92c0RelocAddress = 0x2dcb178",
            "kVtable2dcb170Slot08_6e92c0IdValue = -256",
        ],
        "sites": [0x6A2DC4, 0x6A4ADA],
    },
    {
        "rva": 0x9804C0,
        "size": 0x155,
        "body": (
            "554889e54157415641554154534883ec38488b0540e0c70048894db8488955c04c8945b049"
            "89fd488d4dccba000100004889f7488b00488945d0488d05265093004889c6e8a799fdff8b"
            "45ccb900040000ba010000004c89ef3d000400000f46c885c00f44caba0100000089c8894d"
            "cc4869f070060000e8e490fdff8b75ccba010000004c89ef4889c348c1e607e8cd90fdff8b"
            "75cc4885db746c85f674684531e44531f648895da80f1f80000000004489f131f6488d1524"
            "3300004c69e9700600004e8d3c2b4a8dbc2b380500004889c34c89f9e8663701004889d848"
            "8b5da84489e141ffc64183c410488d0cc84e89bc2b200500004a898c2b180600008b75cc41"
            "39f672a9488b4dc04885c97403488919488b55b8488b4db04885d2740748c7027006000048"
            "85c974028931488b0523dfc700488b00483b45d0751131c04883c4385b415c415d415e415f"
            "5dc3e88ced600090"
        ),
        "sha": "dd32643af50b67bbdfac857f86176cccd79f239d3f5b4a59169d56f11e72e9bc",
        "table": 0x2DD09B0,
        "slot": 0x08,
        "reloc": 0x2DD09B8,
        "header": ROOT / "native/core/vtable_2dd09b0_slot08_9804c0.hpp",
        "needles": [
            "kVtable2dd09b0Slot08_9804c0Rva = 0x9804c0",
            "kVtable2dd09b0Slot08_9804c0Size = 0x155",
            "kVtable2dd09b0Slot08_9804c0TableAddress = 0x2dd09b0",
            "kVtable2dd09b0Slot08_9804c0SlotOffset = 0x08",
            "kVtable2dd09b0Slot08_9804c0RelocAddress = 0x2dd09b8",
            "kVtable2dd09b0Slot08_9804c0RecordSize = 0x670",
        ],
        "sites": [0x96C533],
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
    table_header = TABLE_HEADER.read_text()

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

        slot_value = struct.unpack("<Q", img.read(unit["table"] + unit["slot"], 8))[0]
        require(slot_value == rva, f"{rva:#x} slot qword is {slot_value:#x}")
        rows = [row for row in relocs if row["target"] == unit["reloc"]]
        require(len(rows) == 1, f"{rva:#x} expects one RELA row at {unit['reloc']:#x}")
        require(rows[0]["type"] == 8, f"{rva:#x} expects a type-8 relocation")
        require(rows[0]["addend"] == rva, f"{rva:#x} RELA addend")

        if fdes is not None:
            found = fdes.find(rva)
            require(found is not None and found[0] == rva, f"{rva:#x} is not an FDE start")
            require(found[1] == unit["size"], f"{rva:#x} FDE length {found[1]:#x}")

        header = Path(unit["header"]).read_text()
        for needle in unit["needles"]:
            require(needle in header, f"{rva:#x} header constant missing: {needle}")

        # Every recorded call site must be a table row with the same table, slot and target.
        for site in unit["sites"]:
            needle = (f"{{0x{site:x}, kResolvedVtable_{unit['table']:x}, "
                      f"0x{unit['slot']:02x}, kResolvedVtableTarget_{rva:x}Rva}}")
            require(needle in table_header, f"missing table row: {needle}")

        key = f"0x{rva:x}"
        require(key in entry_map, f"entry map is missing {key}; run tools/lift/gen_entry_map.py")
        owners = {row["header"] for row in entry_map[key]}
        module = f"native/core/{Path(unit['header']).name}"
        require(module in owners or "native/core/vtable_resolved_entries.hpp" in owners,
                f"{key} entry map does not name the unit header")

    for needle in (
        'core/vtable_15cf248_slot68_9b7b80.cpp',
        'core/vtable_15de228_slot20_d7c4b0.cpp',
        'core/vtable_2dcb170_slot08_6e92c0.cpp',
        'core/vtable_2dd09b0_slot08_9804c0.cpp',
        'test_vtable_frontier2',
    ):
        require(needle in cmake, f"cmake missing {needle}")

    total_sites = sum(len(unit["sites"]) for unit in UNITS)
    print(f"OK check_vtable_frontier2: {len(UNITS)} bodies, {total_sites} call sites, "
          f"slots, RELA rows and header constants agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
