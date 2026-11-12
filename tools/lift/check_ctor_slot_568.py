#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 568 (RVA 0x207e0, 782 B).

Reads the pinned private ELF only; never loads, relocates or executes an original RVA.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf, ROOT, TABLE_START, require  # noqa: E402

BASE = 0x207E0
SIZE = 0x30E
SHA = "95b7a07e322f925f04be81bd159802073e40c6eec3a309cadf3808d4a684719b"
PAD = "90" * 2

CALLEES = [
    (0xF2F0, 0x28, "0173e3a3f11a8aa020b87731d2eef5026621d6f1fa0967581b0dab3df646973b", "f2f0"),
    (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45", "20af0"),
    (0x20540, 253, "52da25ce055567a6aae236502dbc1f108a52fba1c8ab231d7d02a9f9bc8e34e0", "20540"),
    (0xF730, 0x72, "03645245094b2faf0ccf36aaa9ec2fa186eca6d01ac1162203b7e0af5d41bb78", "f730"),
    (0x20640, 342, "791da96d41143ba501b767298b123f344debadd18b7c5f06908f030d3897f343", "20640"),
    (0xF180, 28, "bd33279f6055c44aae01e0f9cdcb392acdadfcf4749dc6fe6e6113f5940f714c", "f180"),
    (0xF590, 0x49, "55eef47ed634189652e4cfc5397d8fa8ee58de4385a6cddc30f403de7ba40a47", "f590"),
    (0x4B0, 0x71, "a15bae38e545531a8c5769a079333bd4e0385846160e10a4b768827ecb0d8c8c", "4b0"),
    (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403", "20d60"),
    (0x207D0, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e", "207d0"),
]

IMM_PINS = [
    (0x207ED, "4883e4e0", "32-byte stack alignment"),
    (0x207F1, "4881ec00010000", "0x100-byte frame"),
    (0x2083E, "b902000000", "command flags 2"),
    (0x20848, "48c744243031000000", "description length 0x31"),
    (0x208D5, "be02000000", "reserve count 2"),
    (0x208E7, "49c7879000000000000000", "zero vector capacity"),
    (0x20931, "c744246804000000", "file view length 4"),
    (0x2094C, "c70704000000", "first token kind 4"),
    (0x20969, "4883c728", "empty view at token+0x28"),
    (0x20A03, "48c744247006000000", "inline view length 6"),
    (0x20A25, "c70700000000", "second token kind 0"),
    (0x20A7C, "c640f800", "second token +0x48 = 0"),
    (0x20A93, "ff5010", "command vtable +0x10 register"),
    (0x20A9E, "ff5318", "scratch vtable +0x18 no-op"),
    (0x20AE7, "c3", "canary-matched return"),
]

RIP_PINS = [
    (0x207F8, 7, 3, 0x15FE518, "canary cell"),
    (0x20804, 7, 3, 0xF959CE, "LoadCmdsFromFile"),
    (0x20821, 7, 3, 0xF959EC, "description"),
    (0x2082C, 7, 3, 0x1FE80, "command handler"),
    (0x20833, 7, 3, 0x2DFCA88, "command object"),
    (0x20857, 8, 4, 0x15FE5E0, "tail word pair"),
    (0x20863, 7, 3, 0x20350, "scratch tag"),
    (0x20883, 7, 3, 0x1594D60, "scratch vtable"),
    (0x2089C, 7, 3, 0x1594DC0, "command vtable base"),
    (0x208FA, 7, 3, 0xF959E6, "file literal"),
    (0x209E8, 7, 3, 0xF959DF, "inline literal"),
    (0x20AB5, 7, 3, 0x4B0, "command destructor"),
    (0x20ABC, 7, 3, 0x1604218, "__dso_handle"),
]

CALL_PINS = [
    (0x2081C, 0xF2F0, "name view"),
    (0x20897, 0x20AF0, "command base"),
    (0x208F5, 0x20540, "reserve two tokens"),
    (0x20922, 0xF730, "empty static"),
    (0x2096D, 0xF180, "first empty view"),
    (0x20996, 0x20640, "first grow/push"),
    (0x209AB, 0xF590, "first empty release"),
    (0x209DC, 0xF590, "file view release"),
    (0x20A42, 0xF180, "second empty view"),
    (0x20A6B, 0x20640, "second grow/push"),
    (0x20A80, 0xF590, "inline view release"),
    (0x20AA6, 0xF590, "description release"),
    (0x20AB0, 0xF590, "name release"),
    (0x20AC6, 0xF8F390, "atexit"),
    (0x20AE8, 0xF8F3A0, "canary fail"),
]

STRINGS = [
    (0xF959CE, "LoadCmdsFromFile"),
    (0xF959EC, "Load and dispatch ConVars/ConCommands from a file"),
    (0xF959E6, "file"),
    (0xF959DF, "inline"),
]

CONSTS = {
    "kSlot568Rva": BASE,
    "kSlot568Size": SIZE,
    "kSlot568TableWord": 0x2DD5F20,
    "kSlot568CommandObject": 0x2DFCA88,
    "kSlot568NameViewRva": 0xF2F0,
    "kSlot568NameViewSize": 0x28,
    "kSlot568EmptyViewRva": 0xF180,
    "kSlot568EmptyViewSize": 28,
    "kSlot568EmptyStaticRva": 0xF730,
    "kSlot568EmptyStaticSize": 0x72,
    "kSlot568ReserveRva": 0x20540,
    "kSlot568ReserveSize": 253,
    "kSlot568PushRva": 0x20640,
    "kSlot568PushSize": 342,
    "kSlot568EmptyObject": 0x2DD9F50,
    "kSlot568EmptyData": 0x2DD9F78,
    "kSlot568CommandVptr": 0x1594DD0,
    "kSlot568CommandVtable": 0x1594DC0,
    "kSlot568CommandRegisterRva": 0x20D60,
    "kSlot568CommandTail8": 0x1990,
    "kSlot568CommandTail10": 0x2512A0,
    "kSlot568CommandHandler": 0x1FE80,
    "kSlot568CommandDtor": 0x4B0,
    "kSlot568CommandFlags": 2,
    "kSlot568CommandSize": 0xA0,
    "kSlot568TokenCount": 2,
    "kSlot568ScratchVtable": 0x1594D60,
    "kSlot568ScratchTag": 0x20350,
    "kSlot568NoopRva": 0x207D0,
    "kSlot568ScratchSize": 0x30,
    "kSlot568NameLength": 16,
    "kSlot568DescriptionLength": 0x31,
    "kSlot568FileLength": 4,
    "kSlot568InlineLength": 6,
}

HEADER_TOKENS = [
    "0x207e0", "0x2dd5f20", "0x2dfca88", "0x2dd9f78", "0x1594dd0", "0x1594d60",
    "0x207d0", "Slot568Token", "host-owned", "first token's +0x48",
]
SOURCE_TOKENS = [
    "init_concmd_base_20af0", "register_concmd_20d60", "release_inert_view_f590",
    "empty_string_f730", "first.empty = empty_literal_view", "objects.scratch.vtable",
    "out.first_flag_written = false", "out.grow_path = false",
]
TEST_TOKENS = [
    "first.flag48 == 0xa5", "command.vec_begin == &objects.tokens[0]",
    "second.flag48 == 0", "registry.pending.size() == 1", "registry.atexit_log.size() == 1",
    "section_hook_failure",
]
CMAKE_TOKENS = ["core/ctor_slot_568_207e0.cpp", "test_ctor_slot_568.cpp"]


def run_elf(elf: Elf) -> None:
    require(elf.u64(TABLE_START - 8 * 568) == BASE, "constructor table word 568")
    elf.body_hash("ctor_slot_568", BASE, SIZE, SHA)
    elf.bytes_at(BASE + SIZE, PAD, "two nops after slot 568")
    for rva, size, sha, label in CALLEES:
        elf.body_hash(label, rva, size, sha, always=True)
    for addr, hexbytes, label in IMM_PINS:
        elf.bytes_at(addr, hexbytes, label)
    for site, length, disp_off, target, label in RIP_PINS:
        require(elf.rip(site, length, disp_off) == target, f"{label}: {target:#x}")
    for site, target, label in CALL_PINS:
        require(elf.call(site) == target, f"{label}: {target:#x}")
    for rva, text in STRINGS:
        elf.cstring(rva, text.encode())
    # The two vtable dispatches are resolved from the pinned tables, never executed.
    require(elf.u64(0x1594DD0 + 0x10) == 0x20D60, "command vtable +0x10 -> 0x20d60")
    require(elf.u64(0x1594D60 + 0x18) == 0x207D0, "scratch vtable +0x18 -> 0x207d0")
    require(elf.u64(0x15FE5E0) == 0x1990, "tail8")
    require(elf.u64(0x15FE5E8) == 0x2512A0, "tail10")


def native_ok(header: str, source: str, test: str, cmake: str) -> None:
    for name, value in CONSTS.items():
        match = re.search(rf"\b{name}\s*=\s*(0x[0-9A-Fa-f]+|\d+)", header)
        require(match is not None, f"native constant {name} missing")
        require(int(match.group(1), 0) == value, f"native constant {name} == {value:#x}")
    for token in HEADER_TOKENS:
        require(token in header, f"header token {token!r}")
    for token in SOURCE_TOKENS:
        require(token in source, f"source token {token!r}")
    for token in TEST_TOKENS:
        require(token in test, f"test token {token!r}")
    for token in CMAKE_TOKENS:
        require(token in cmake, f"cmake token {token!r}")


def mutation_self_test(elf: Elf, header: str, source: str, test: str, cmake: str) -> int:
    ran = 0
    for off in (0, 1, 0x20, 0x100, SIZE - 2):
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
    for rva, _ in STRINGS[:3]:
        elf.overlay = {rva: elf.raw(rva, 1)[0] ^ 0x01}
        ran += 1
        try:
            run_elf(elf)
        except SystemExit:
            continue
        raise SystemExit(f"FAIL string mutant survived at {rva:#x}")
    elf.overlay = {}
    mutated = header.replace("kSlot568Size = 0x30e;", "kSlot568Size = 0x30f;", 1)
    require(mutated != header, "--tamper-native could not edit the header")
    try:
        native_ok(mutated, source, test, cmake)
    except SystemExit:
        ran += 1
    else:
        raise SystemExit("FAIL native size mutant survived")
    return ran


def main() -> int:
    args = sys.argv[1:]
    elf = Elf()
    header = (ROOT / "native/core/ctor_slot_568_207e0.hpp").read_text()
    source = (ROOT / "native/core/ctor_slot_568_207e0.cpp").read_text()
    test = (ROOT / "native/tests/test_ctor_slot_568.cpp").read_text()
    cmake = (ROOT / "native/CMakeLists.txt").read_text()

    if "--tamper-native" in args:
        mutated = header.replace("kSlot568Size = 0x30e;", "kSlot568Size = 0x30f;", 1)
        if mutated == header:
            raise SystemExit("FAIL --tamper-native could not edit the header")
        native_ok(mutated, source, test, cmake)
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
    native_ok(header, source, test, cmake)
    if "--no-mutate" not in args:
        ran = mutation_self_test(elf, header, source, test, cmake)
        print(f"OK check_ctor_slot_568: 782 B hash, {len(CALLEES)} callee hashes, "
              f"{len(RIP_PINS)} object/rip pins, {len(CALL_PINS)} calls, {len(STRINGS)} strings; "
              f"{ran} mutants rejected")
    return 0


if __name__ == "__main__":
    sys.exit(main())
