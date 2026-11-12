#!/usr/bin/env python3
"""ELF byte-oracle for the scripted-entry helper 0x3c2a00 (398 B).

Reads the pinned private ELF only; never loads, relocates or executes an original RVA.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf, ROOT, require  # noqa: E402

BASE = 0x3C2A00
SIZE = 398
SHA = "3e00391a7ce023395c15260dc6ed2e56247c386342a4d6102c8a5f5106a79e59"

IMM_PINS = [
    (0x3C2A18, "c5fc1103", "zero first descriptor"),
    (0x3C2A39, "48891b", "descriptor 0 self pointer"),
    (0x3C2A60, "488d4320", "descriptor 1 self pointer"),
    (0x3C2A64, "488900", "descriptor 1 self store"),
    (0x3C2A8F, "488d4340", "descriptor 2 self pointer"),
    (0x3C2A93, "488900", "descriptor 2 self store"),
    (0x3C2A9A, "c5f8114360", "name view copy"),
    (0x3C2AAB, "48894378", "name ctl store"),
    (0x3C2AB4, "f0", "name ctl lock prefix"),
    (0x3C2AB5, "48ff00", "name ctl increment"),
    (0x3C2AC2, "48898b80000000", "previous list head store"),
    (0x3C2AD0, "488918", "global list head = entry"),
    (0x3C2AD3, "4c8b31", "load registry cell"),
    (0x3C2AD6, "4d85f6", "null registry guard"),
    (0x3C2B68, "4c39e9", "lower_bound sentinel compare"),
    (0x3C2B6D, "4883794000", "mapped-value null compare"),
    (0x3C2B7C, "488918", "store entry into mapped slot"),
]

RIP_PINS = [
    (0x3C2AB8, 7, 3, 0x2EF6400, "lea global list head"),
    (0x3C2AC9, 7, 3, 0x2EF63F8, "lea registry cell"),
]

CALL_PINS = [
    (0x3C2B17, 0xF8F500, "lower_bound strcasecmp"),
    (0x3C2B56, 0xF8F500, "mapped-key strcasecmp"),
    (0x3C2B77, 0x3C0840, "find-or-insert on missing/null mapping"),
]

CONSTS = {
    "kScriptedEntryCtorRva": 0x3C2A00,
    "kScriptedEntryCtorSize": 398,
    "kScriptedEntryListHeadCellRva": 0x2EF6400,
    "kScriptedEntryRegistryCellRva": 0x2EF63F8,
    "kScriptedEntryStrcasecmpPltRva": 0xF8F500,
    "kScriptedEntrySize": 0x90,
}

HEADER_TOKENS = [
    "0x3c2a00", "0x2ef6400", "0x2ef63f8", "0xf8f500", "0x3c0840",
    "ScriptedEntryDescriptor", "tail88", "rb_registry_3be2d0", "identity",
]
SOURCE_TOKENS = [
    "entry.groups[i] = descriptors[i]", "concmd_acquire_view(entry.name)",
    "entry.next = list_head", "list_head = &entry", "rb_index_3c0840",
    "*index.slot = &entry", "out.ok = true",
]
TEST_TOKENS = ["section_lists", "section_registry", "section_registry_failure",
               "entry.tail88 == 0xA5A5A5A5A5A5A5A5", "duplicate.already_present"]


def run_elf(elf: Elf) -> None:
    elf.body_hash("scripted_entry_3c2a00", BASE, SIZE, SHA)
    for addr, hexbytes, label in IMM_PINS:
        elf.bytes_at(addr, hexbytes, label)
    for site, length, disp_off, target, label in RIP_PINS:
        require(elf.rip(site, length, disp_off) == target, f"{label}: {target:#x}")
    for site, target, label in CALL_PINS:
        require(elf.call(site) == target, f"{label}: {target:#x}")


def native_ok(header: str, source: str, test: str = "") -> None:
    for name, value in CONSTS.items():
        match = re.search(rf"\b{name}\s*=\s*(0x[0-9A-Fa-f]+|\d+)", header)
        require(match is not None, f"native constant {name} missing")
        require(int(match.group(1), 0) == value, f"native constant {name} == {value:#x}")
    for token in HEADER_TOKENS:
        require(token in header, f"header token {token!r}")
    for token in SOURCE_TOKENS:
        require(token in source, f"source token {token!r}")
    if test:
        for token in TEST_TOKENS:
            require(token in test, f"test token {token!r}")


def mutation_self_test(elf: Elf, header: str, source: str, test: str) -> int:
    ran = 0
    for off in (0, 1, 0x20, SIZE - 2):
        elf.overlay = {BASE + off: elf.raw(BASE + off, 1)[0] ^ 0x01}
        ran += 1
        try:
            run_elf(elf)
        except SystemExit:
            continue
        raise SystemExit(f"FAIL hash mutant survived at {BASE + off:#x}")
    for addr, _, _ in IMM_PINS:
        elf.overlay = {addr: elf.raw(addr, 1)[0] ^ 0x01}
        ran += 1
        try:
            run_elf(elf)
        except SystemExit:
            continue
        raise SystemExit(f"FAIL imm mutant survived at {addr:#x}")
    for site, _, _, _, _ in RIP_PINS:
        elf.overlay = {site: elf.raw(site, 1)[0] ^ 0x01}
        ran += 1
        try:
            run_elf(elf)
        except SystemExit:
            continue
        raise SystemExit(f"FAIL rip mutant survived at {site:#x}")
    for site, _, _ in CALL_PINS:
        elf.overlay = {site: elf.raw(site, 1)[0] ^ 0x01}
        ran += 1
        try:
            run_elf(elf)
        except SystemExit:
            continue
        raise SystemExit(f"FAIL call mutant survived at {site:#x}")
    elf.overlay = {}
    mutated = header.replace("kScriptedEntryCtorSize = 398;", "kScriptedEntryCtorSize = 399;", 1)
    require(mutated != header, "--tamper-native could not edit the header")
    try:
        native_ok(mutated, source, test)
    except SystemExit:
        ran += 1
    else:
        raise SystemExit("FAIL native size mutant survived")
    return ran


def main() -> int:
    args = sys.argv[1:]
    elf = Elf()
    header = (ROOT / "native/core/scripted_entry_3c2a00.hpp").read_text()
    source = (ROOT / "native/core/scripted_entry_3c2a00.cpp").read_text()
    test = (ROOT / "native/tests/test_scripted_entry_3c2a00.cpp").read_text()

    if "--tamper-native" in args:
        mutated = header.replace("kScriptedEntryCtorSize = 398;", "kScriptedEntryCtorSize = 399;", 1)
        if mutated == header:
            raise SystemExit("FAIL --tamper-native could not edit the header")
        native_ok(mutated, source, test)
        print("tamper-native NOT detected", file=sys.stderr)
        return 0

    if "--tamper" in args:
        elf.overlay = {BASE + 0x20: elf.raw(BASE + 0x20, 1)[0] ^ 0x80}
        run_elf(elf)
        print("tamper NOT detected", file=sys.stderr)
        return 0

    elf.recording = True
    run_elf(elf)
    elf.recording = False
    native_ok(header, source, test)
    if "--no-mutate" not in args:
        ran = mutation_self_test(elf, header, source, test)
        print(f"OK check_scripted_entry_3c2a00: 398 B hash + {len(IMM_PINS)} imm + "
              f"{len(RIP_PINS)} rip + {len(CALL_PINS)} call pins; {ran} mutants rejected")
    return 0


if __name__ == "__main__":
    sys.exit(main())
