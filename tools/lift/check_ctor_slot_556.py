#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 556 (RVA 0x4ccb0, 1,300 B).

Four `lane_m5` app-info entries plus one `sendEmail` console command. Offline only: reads the pinned
private ELF, never loads, relocates or executes an original RVA.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf, ROOT, TABLE_START, require  # noqa: E402

BASE = 0x4CCB0
SIZE = 0x514
SHA = "f23d9fed626fdaffcda28e56b91cd9744e2c503e65ffe71921cfdab986284953"

CALLEES = [
    (0x4B560, 233, "abd6d52ca9e1933507fb1a3f8d82a22e66d52a47a1348ebc33ef82acb3a7b6c6", "entry_register"),
    (0x4B660, 9, "b97017385f4063c288a65875294606be7cda52205dde185961236f6c8b3ee46b", "entry_dtor"),
    (0x4B650, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e", "app0_handler"),
    (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45", "concmd_base"),
    (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403", "concmd_register"),
    (0xF2F0, 0x28, "0173e3a3f11a8aa020b87731d2eef5026621d6f1fa0967581b0dab3df646973b", "name_view"),
    (0xF590, 0x49, "55eef47ed634189652e4cfc5397d8fa8ee58de4385a6cddc30f403de7ba40a47", "release"),
    (0x4B0, 0x71, "a15bae38e545531a8c5769a079333bd4e0385846160e10a4b768827ecb0d8c8c", "command_dtor"),
]

IMM_PINS = [
    (0x4CCB9, "4883e4e0", "32-byte stack alignment"),
    (0x4CCBD, "4881eca0000000", "0xa0-byte frame"),
    (0x4CCEB, "48c70600000000", "singleton cell = 0"),
    (0x4CD38, "c60000", "entry0 registered = 0"),
    (0x4CD9C, "48c7403800000000", "entry0 delegate high qword = 0"),
    (0x4CE2B, "c60000", "entry1 registered = 0"),
    (0x4CE89, "48c7403800000000", "entry1 delegate high qword = 0"),
    (0x4CF18, "c60000", "entry2 registered = 0"),
    (0x4CF76, "48c7403800000000", "entry2 delegate high qword = 0"),
    (0x4D005, "c60000", "entry3 registered = 0"),
    (0x4D063, "48c7403800000000", "entry3 delegate high qword = 0"),
    (0x4D0ED, "b902000000", "command flags 2"),
    (0x4D0FF, "48c744244833000000", "description length 0x33"),
    (0x4D123, "4889442438", "command handler stack store"),
    (0x4D13E, "48890b", "command vptr store"),
    (0x4D16E, "48c7839000000000000000", "command vec_cap = 0"),
    (0x4D179, "ff5020", "command vtable +0x20 call"),
    (0x4D1B1, "750b", "canary jne"),
    (0x4D1C3, "90", "trailing nop"),
]

RIP_PINS = [
    (0x4CCC4, 7, 3, 0x15FE518, "canary cell"),
    (0x4CCCB, 7, 3, 0x2EA8978, "singleton cell"),
    (0x4CCD2, 7, 3, 0x4A660, "singleton dtor"),
    (0x4CCD9, 7, 3, 0x1604218, "dso"),
    (0x4CCF7, 7, 3, 0xF995E5, "PingAllConnections"),
    (0x4CD0A, 7, 3, 0x2EA8988, "entry0"),
    (0x4CD1F, 8, 4, 0x15FE800, "delegate invoker cell"),
    (0x4CD2C, 7, 3, 0x4B650, "entry0 handler"),
    (0x4CDC2, 7, 3, 0x2EA8988, "entry0 queue target"),
    (0x4CDDB, 7, 3, 0x4B660, "entry dtor"),
    (0x4CDE2, 7, 3, 0x1604218, "dso"),
    (0x4CDF1, 7, 3, 0xF995F8, "SendEngineDispatchCmd"),
    (0x4CE04, 7, 3, 0x2EA89D8, "entry1"),
    (0x4CE1F, 7, 3, 0x4B670, "entry1 handler"),
    (0x4CEAF, 7, 3, 0x2EA89D8, "entry1 queue target"),
    (0x4CEC8, 7, 3, 0x4B660, "entry dtor"),
    (0x4CECF, 7, 3, 0x1604218, "dso"),
    (0x4CEDE, 7, 3, 0xF9960E, "SendConCommand"),
    (0x4CEF1, 7, 3, 0x2EA8A28, "entry2"),
    (0x4CF0C, 7, 3, 0x4B720, "entry2 handler"),
    (0x4CF9C, 7, 3, 0x2EA8A28, "entry2 queue target"),
    (0x4CFB5, 7, 3, 0x4B660, "entry dtor"),
    (0x4CFBC, 7, 3, 0x1604218, "dso"),
    (0x4CFCB, 7, 3, 0xF9961D, "RequestListOfConCommands"),
    (0x4CFDE, 7, 3, 0x2EA8A78, "entry3"),
    (0x4CFF9, 7, 3, 0x4B870, "entry3 handler"),
    (0x4D089, 7, 3, 0x2EA8A78, "entry3 queue target"),
    (0x4D0A2, 7, 3, 0x4B660, "entry dtor"),
    (0x4D0A9, 7, 3, 0x1604218, "dso"),
    (0x4D0BD, 7, 3, 0xF99636, "sendEmail"),
    (0x4D0CE, 7, 3, 0x2EA8AC8, "command"),
    (0x4D0D5, 8, 4, 0x15FE5E0, "tail8 cell"),
    (0x4D0DD, 7, 3, 0xF99640, "command description"),
    (0x4D113, 7, 3, 0x4B980, "command handler"),
    (0x4D12D, 7, 3, 0x1594DC0, "command vtable base"),
    (0x4D190, 7, 3, 0x4B0, "command dtor"),
    (0x4D197, 7, 3, 0x1604218, "dso"),
]

CALL_PINS = [
    (0x4CCF2, 0xF8F390, "singleton atexit"),
    (0x4CD05, 0xF2F0, "entry0 name"),
    (0x4CDCC, 0x4B560, "entry0 queue"),
    (0x4CDD6, 0xF590, "entry0 view release"),
    (0x4CDEC, 0xF8F390, "entry0 atexit"),
    (0x4CDFF, 0xF2F0, "entry1 name"),
    (0x4CEB9, 0x4B560, "entry1 queue"),
    (0x4CEC3, 0xF590, "entry1 view release"),
    (0x4CED9, 0xF8F390, "entry1 atexit"),
    (0x4CEEC, 0xF2F0, "entry2 name"),
    (0x4CFA6, 0x4B560, "entry2 queue"),
    (0x4CFB0, 0xF590, "entry2 view release"),
    (0x4CFC6, 0xF8F390, "entry2 atexit"),
    (0x4CFD9, 0xF2F0, "entry3 name"),
    (0x4D093, 0x4B560, "entry3 queue"),
    (0x4D09D, 0xF590, "entry3 view release"),
    (0x4D0B3, 0xF8F390, "entry3 atexit"),
    (0x4D0C9, 0xF2F0, "command name"),
    (0x4D128, 0x20AF0, "command base"),
    (0x4D181, 0xF590, "command description release"),
    (0x4D18B, 0xF590, "command name release"),
    (0x4D1A1, 0xF8F390, "command atexit"),
    (0x4D1BE, 0xF8F3A0, "canary fail"),
]

STRINGS = [
    (0xF995E5, b"PingAllConnections"),
    (0xF995F8, b"SendEngineDispatchCmd"),
    (0xF9960E, b"SendConCommand"),
    (0xF9961D, b"RequestListOfConCommands"),
    (0xF99636, b"sendEmail"),
    (0xF99640, b"Sends an email. Params: <to> <subject> <opt: body>]"),
]

CONSTS = {
    "kSlot556Rva": 0x4CCB0,
    "kSlot556Size": 0x514,
    "kSlot556TableWord": 0x2DD5F80,
    "kSlot556SingletonCellRva": 0x2EA8978,
    "kSlot556SingletonDtorRva": 0x4A660,
    "kSlot556Entry0Rva": 0x2EA8988,
    "kSlot556EntryStride": 0x50,
    "kSlot556AppCount": 4,
    "kSlot556CommandRva": 0x2EA8AC8,
    "kSlot556EntryRegisterRva": 0x4B560,
    "kSlot556EntryDtorRva": 0x4B660,
    "kSlot556DelegateInvokerRva": 0x4CCA0,
    "kSlot556DelegateInvokerCellRva": 0x15FE800,
    "kSlot556CommandVtableRva": 0x1594DC0,
    "kSlot556CommandVptrRva": 0x1594DD0,
    "kSlot556CommandRegisterSlotRva": 0x1594DE0,
    "kSlot556CommandRegisterRva": 0x20D60,
    "kSlot556CommandDtorRva": 0x4B0,
    "kSlot556CommandFlags": 2,
    "kSlot556CommandSize": 0xA0,
    "kSlot556CommandTail8": 0x1990,
    "kSlot556CommandTail10": 0,
    "kSlot556CommandHandler": 0x4B980,
    "kSlot556CommandNameLength": 9,
    "kSlot556CommandDescriptionLength": 51,
}

HEADER_TOKENS = [
    "PingAllConnections", "SendEngineDispatchCmd", "SendConCommand", "RequestListOfConCommands",
    "sendEmail", "kSlot556Apps", "0x4b980", "0x4a660", "0x4b560",
]
SOURCE_TOKENS = [
    "queue.singleton = nullptr", "init_named_delegate_entry", "init_concmd_base_20af0",
    "register_concmd_20d60", "command.vec_begin = 0", "out.command_gap98_written = false",
    "out.atexit_records == 6",
]
TEST_TOKENS = [
    "queue.singleton == nullptr", "queue.head == &objects.apps[3]", "command.gap98 == kPoison64",
    "registry.atexit_log.size() == 6", "section_alone",
]
CMAKE_TOKENS = ["core/ctor_slot_556_4ccb0.cpp", "test_ctor_slot_556.cpp"]


def run_elf(elf: Elf) -> None:
    require(elf.u64(TABLE_START - 8 * 556) == BASE, "constructor table word 556")
    elf.body_hash("ctor_slot_556", BASE, SIZE, SHA)
    for rva, size, sha, label in CALLEES:
        elf.body_hash(label, rva, size, sha, always=True)
    for addr, hexbytes, label in IMM_PINS:
        elf.bytes_at(addr, hexbytes, label)
    for site, length, disp_off, target, label in RIP_PINS:
        require(elf.rip(site, length, disp_off) == target, f"{label}: {target:#x}")
    for site, target, label in CALL_PINS:
        require(elf.call(site) == target, f"{label}: {target:#x}")
    for rva, text in STRINGS:
        elf.cstring(rva, text)
    require(elf.u64(0x1594DE0) == 0x20D60, "command vtable +0x20 -> 0x20d60")
    require(elf.u64(0x1594DD0) == 0x4B0, "command vptr -> 0x4b0")
    require(elf.u64(0x15FE800) == 0x4CCA0, "delegate invoker cell -> 0x4cca0")
    require(elf.u64(0x15FE5E0) == 0x1990, "command tail8 cell -> 0x1990")
    elf.bytes_at(0x4B650, "c3", "entry0 handler is a one-byte ret")


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
    for off in (0, 1, 0x20, 0x200, 0x300, SIZE - 1):
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
    for rva, _ in STRINGS[:5]:
        elf.overlay = {rva: elf.raw(rva, 1)[0] ^ 0x01}
        ran += 1
        try:
            run_elf(elf)
        except SystemExit:
            continue
        raise SystemExit(f"FAIL string mutant survived at {rva:#x}")
    elf.overlay = {}
    mutated = header.replace("kSlot556Size = 0x514;", "kSlot556Size = 0x515;", 1)
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
    header = (ROOT / "native/core/ctor_slot_556_4ccb0.hpp").read_text()
    source = (ROOT / "native/core/ctor_slot_556_4ccb0.cpp").read_text()
    test = (ROOT / "native/tests/test_ctor_slot_556.cpp").read_text()
    cmake = (ROOT / "native/CMakeLists.txt").read_text()

    if "--tamper-native" in args:
        mutated = header.replace("kSlot556Size = 0x514;", "kSlot556Size = 0x515;", 1)
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
        print(f"OK check_ctor_slot_556: 1300 B hash, {len(CALLEES)} callee hashes, "
              f"{len(RIP_PINS)} object/rip pins, {len(CALL_PINS)} calls, {len(STRINGS)} strings; "
              f"{ran} mutants rejected")
    return 0


if __name__ == "__main__":
    sys.exit(main())
