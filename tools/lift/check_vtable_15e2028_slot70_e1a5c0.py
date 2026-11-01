#!/usr/bin/env python3
"""ELF tie for the resolved lookup target 0xe1a5c0 (vtable 0x15e2028 slot 0x70)."""

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
RVA = 0xE1A5C0
SIZE = 0x97
END = 0xE1A657
BODY_SHA256 = "b8b5e26419fc0cec66ebcde09412e47717763c78a40a2b455250c55f74e9fa6e"
VTABLE = 0x15E2028
SLOT = 0x70
RELOC = 0x15E2098
STRCMP = 0xF8F490
HEADER = ROOT / "native/core/vtable_15e2028_slot70_e1a5c0.hpp"
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
        ("41574156415453", "saves r15/r14/r12/rbx"),
        ("4989f7", "arg1 -> r15"),
        ("4189d6", "arg2 -> r14d"),
        ("4989fc", "this -> r12"),
        ("4d85ff", "null-key test"),
        ("41837c241000", "count load at +0x10"),
        ("498b442408", "array load at +0x8"),
        ("488b3cd8", "element load"),
        ("ff5010", "element key slot 0x10"),
        ("4885c0", "null-key skip test"),
        ("4889c74c89fe", "strcmp arguments"),
        ("ff5018", "element value slot 0x18"),
        ("4439f0", "value compare"),
        ("ffc3", "increment"),
        ("413b5c2410", "count compare"),
        ("b801000000", "not-found return 1"),
        ("5b415c415e415f5dc3", "epilogue and ret"),
        ("ff6020", "tail call slot 0x20"),
    ]
    for needle, label in anchors:
        require(bytes.fromhex(needle) in body, f"missing {label}")

    require(body.count(bytes.fromhex("ff5010")) == 1, "expected one key-slot call")
    require(body.count(bytes.fromhex("ff5018")) == 1, "expected one value-slot call")
    require(body.count(bytes.fromhex("ff6020")) == 1, "expected one tail slot call")

    strcmp_call = body.find(bytes.fromhex("e8"))
    # The strcmp call is the only relative call in the body; resolve it.
    relative_calls = [i for i in range(len(body) - 5) if body[i] == 0xE8]
    targets = [RVA + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] for i in relative_calls]
    require(targets == [STRCMP], f"unexpected relative call targets: {[hex(t) for t in targets]}")

    require(img.read(END, 4) == bytes.fromhex("90909090"), "padding after FDE end")

    slot_value = struct.unpack("<Q", img.read(VTABLE + SLOT, 8))[0]
    require(slot_value == RVA, "vtable slot does not point at 0xe1a5c0")

    rows = [row for row in d.parse(elf)["relocs"] if row["target"] == RELOC]
    require(len(rows) == 1, f"expected one RELA row at {RELOC:#x}")
    require(rows[0]["type"] == 8 and rows[0]["addend"] == RVA, "RELA type/addend")

    header = HEADER.read_text()
    for needle in (
        "kVtable15e2028Slot70_e1a5c0Rva = 0xe1a5c0",
        "kVtable15e2028Slot70_e1a5c0Size = 0x97",
        "kVtable15e2028Slot70_e1a5c0VtableAddress = 0x15e2028",
        "kVtable15e2028Slot70_e1a5c0SlotOffset = 0x70",
        "kVtable15e2028Slot70_e1a5c0RelocAddress = 0x15e2098",
        "kVtable15e2028Slot70_e1a5c0CountOffset = 0x10",
        "kVtable15e2028Slot70_e1a5c0ArrayOffset = 0x08",
        "kVtable15e2028Slot70_e1a5c0StrcmpTarget = 0xf8f490",
    ):
        require(needle in header, f"header constant missing: {needle}")

    require("vtable_15e2028_slot70_e1a5c0.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_15e2028_slot70" in CMAKE.read_text(), "cmake test")
    require("0xe1a5c0" in json.loads(ENTRY_MAP.read_text())["entries"], "entry map missing 0xe1a5c0")

    print("OK check_vtable_15e2028_slot70_e1a5c0: scan structure, strcmp target, slot, RELA and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
