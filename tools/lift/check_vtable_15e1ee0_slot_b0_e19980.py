#!/usr/bin/env python3
"""ELF tie for the resolved alignment-dispatch target 0xe19980 (vtable 0x15e1ee0 slot 0xb0)."""

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
RVA = 0xE19980
SIZE = 0xA2
END = 0xE19A22
BODY_SHA256 = "ed80156458b2ad18c44088883d9b9b2a09cc9f40254bb8a34477f9943d8898fd"
VTABLE = 0x15E1EE0
SLOT = 0xB0
RELOC = 0x15E1F90
FORMAT = 0xB30570
FAST_HELPER = 0xE1EDA0
STACK_CHK_FAIL = 0xF8F3A0
HEADER = ROOT / "native/core/vtable_15e1ee0_slot_b0_e19980.hpp"
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
        ("4881ec10010000", "0x110 frame"),
        ("4c8b25", "canary cell load"),
        ("4889d3", "arg3 -> rbx"),
        ("4989f6", "arg2 -> r14"),
        ("f6c307", "arg3 alignment test"),
        ("f6c107", "arg4 alignment test"),
        ("ff90d8000000", "slow vcall slot 0xd8"),
        ("488d05649a5302", "fast global lea"),
        ("488903", "fast global store"),
        ("483b45d8", "canary compare"),
        ("4881c4100100005b415c415e415f5dc3", "epilogue"),
    ]
    for needle, label in anchors:
        require(bytes.fromhex(needle) in body, f"missing {label}")

    relative_calls = [i for i in range(len(body) - 5) if body[i] == 0xE8]
    targets = sorted(RVA + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0]
                     for i in relative_calls)
    require(targets == sorted([FORMAT, FAST_HELPER, STACK_CHK_FAIL]),
            f"unexpected relative call targets: {[hex(t) for t in targets]}")

    require(img.read(END, 4) == bytes.fromhex("90909090"), "padding after FDE end")

    slot_value = struct.unpack("<Q", img.read(VTABLE + SLOT, 8))[0]
    require(slot_value == RVA, "vtable slot does not point at 0xe19980")

    rows = [row for row in d.parse(elf)["relocs"] if row["target"] == RELOC]
    require(len(rows) == 1, f"expected one RELA row at {RELOC:#x}")
    require(rows[0]["type"] == 8 and rows[0]["addend"] == RVA, "RELA type/addend")

    header = HEADER.read_text()
    for needle in (
        "kVtable15e1ee0SlotB0_e19980Rva = 0xe19980",
        "kVtable15e1ee0SlotB0_e19980Size = 0xa2",
        "kVtable15e1ee0SlotB0_e19980VtableAddress = 0x15e1ee0",
        "kVtable15e1ee0SlotB0_e19980SlotOffset = 0xb0",
        "kVtable15e1ee0SlotB0_e19980RelocAddress = 0x15e1f90",
        "kVtable15e1ee0SlotB0_e19980CanaryAddress = 0x15fe518",
        "kVtable15e1ee0SlotB0_e19980FormatRva = 0xb30570",
        "kVtable15e1ee0SlotB0_e19980FastHelperRva = 0xe1eda0",
        "kVtable15e1ee0SlotB0_e19980FastStoreGlobal = 0x3353478",
        "kVtable15e1ee0SlotB0_e19980SlowSlot = 0xd8",
    ):
        require(needle in header, f"header constant missing: {needle}")

    require("vtable_15e1ee0_slot_b0_e19980.cpp" in CMAKE.read_text(), "cmake source")
    require("test_vtable_15e1ee0_slot_b0" in CMAKE.read_text(), "cmake test")
    require("0xe19980" in json.loads(ENTRY_MAP.read_text())["entries"], "entry map missing 0xe19980")

    print("OK check_vtable_15e1ee0_slot_b0_e19980: alignment dispatch, canary, slot, RELA and entry map agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
