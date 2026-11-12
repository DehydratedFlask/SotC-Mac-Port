#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 468 (RVA 0x3c3a90, 4,772 B).

Reads the pinned private ELF only; never loads, relocates or executes an original RVA.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf, ROOT, TABLE_START, require  # noqa: E402

BASE = 0x3C3A90
SIZE = 4772
SHA = "26e0067a3883f73768e4ca64f4cb64aa4c962a1bd374d996f483aac3fd1c7d82"
PAD = "90" * 12

CALLEES = [
    (0x4B560, 233, "abd6d52ca9e1933507fb1a3f8d82a22e66d52a47a1348ebc33ef82acb3a7b6c6", "4b560"),
    (0x3C2A00, 398, "3e00391a7ce023395c15260dc6ed2e56247c386342a4d6102c8a5f5106a79e59", "3c2a00"),
    (0x20540, 253, "52da25ce055567a6aae236502dbc1f108a52fba1c8ab231d7d02a9f9bc8e34e0", "20540"),
    (0x20640, 342, "791da96d41143ba501b767298b123f344debadd18b7c5f06908f030d3897f343", "20640"),
    (0xF180, 28, "bd33279f6055c44aae01e0f9cdcb392acdadfcf4749dc6fe6e6113f5940f714c", "f180"),
    (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45", "20af0"),
    (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403", "20d60"),
    (0x4B0, 0x71, "a15bae38e545531a8c5769a079333bd4e0385846160e10a4b768827ecb0d8c8c", "4b0"),
    (0xF590, 0x49, "55eef47ed634189652e4cfc5397d8fa8ee58de4385a6cddc30f403de7ba40a47", "f590"),
    (0x3C15E0, 9, "cdf18c3b3e3c6ee381e0449f5e2ca37c0889cca89616aee15bb58b94697b8af6", "3c15e0"),
    (0x4B660, 30, "a372e93a87e711ef2857428b5947d9f3dced5f93206d87c8a3644e609a37624f", "4b660"),
]

IMM_PINS = [
    (0x3C430F, "41ffd5", "ordinary command register through %r13"),
    (0x3C4733, "c744247c00000000", "rewind token kind 0"),
    (0x3C4743, "48c78424a800000005000000", "rewind token text length 5"),
    (0x3C47C5, "c640f801", "rewind token +0x48 = 1"),
    (0x3C47DC, "ff5010", "rewind vtable +0x10 registration"),
    (0x3C47E7, "ff5318", "rewind identity cell +0x18"),
    (0x3C4D2D, "c3", "canary-matched return"),
]

RIP_PINS = [
    (0x3C3AD0, 7, 3, 0x2EF6408, "app entry 0"),
    (0x3C3BE2, 7, 3, 0x2EF6458, "app entry 1"),
    (0x3C3CE4, 7, 3, 0x2EF64A8, "app entry 2"),
    (0x3C3E1D, 7, 3, 0x2EF64F8, "scripted entry 0"),
    (0x3C3EFF, 7, 3, 0x2EF6588, "scripted entry 1"),
    (0x3C3FDA, 7, 3, 0x2EF6618, "scripted entry 2"),
    (0x3C40B5, 7, 3, 0x2EF66A8, "scripted entry 3"),
    (0x3C4193, 7, 3, 0x2EF6738, "scripted entry 4"),
    (0x3C4247, 7, 3, 0x2EF67C8, "command 0"),
    (0x3C435B, 7, 3, 0x2EF6868, "command 1"),
    (0x3C4458, 7, 3, 0x2EF6908, "command 2"),
    (0x3C4555, 7, 3, 0x2EF69A8, "command 3"),
    (0x3C4664, 7, 3, 0x2EF6A48, "command 4"),
    (0x3C4833, 7, 3, 0x2EF6AE8, "command 5"),
    (0x3C4930, 7, 3, 0x2EF6B88, "command 6"),
    (0x3C4A2D, 7, 3, 0x2EF6C28, "command 7"),
    (0x3C4B2A, 7, 3, 0x2EF6CC8, "command 8"),
    (0x3C4C27, 7, 3, 0x2EF6D68, "command 9"),
    (0x3C42BC, 7, 3, 0x1594DC0, "command vtable"),
    (0x3C46B4, 7, 3, 0x15B0E10, "rewind identity cell"),
    (0x3C4728, 7, 3, 0xFE7955, "pause literal"),
    (0x3C4D0B, 7, 3, 0x15FE518, "canary cell"),
]

CALL_PINS = [
    (0x3C3ACB, 0xF2F0, "app 0 name view"),
    (0x3C3BA4, 0x4B560, "app 0 queue"),
    (0x3C3BB1, 0xF590, "app 0 view"),
    (0x3C3BC7, 0xF8F390, "app 0 atexit"),
    (0x3C3BDD, 0xF2F0, "app 1 name view"),
    (0x3C3CA6, 0x4B560, "app 1 queue"),
    (0x3C3CC9, 0xF8F390, "app 1 atexit"),
    (0x3C3CDF, 0xF2F0, "app 2 name view"),
    (0x3C3DA8, 0x4B560, "app 2 queue"),
    (0x3C3DCE, 0xF8F390, "app 2 atexit"),
    (0x3C3E8B, 0x3C2A00, "entry 0 helper"),
    (0x3C3E93, 0xF590, "entry 0 description view"),
    (0x3C3E9B, 0xF590, "entry 0 name view"),
    (0x3C3EB0, 0xF8F390, "entry 0 atexit"),
    (0x3C3F6D, 0x3C2A00, "entry 1 helper"),
    (0x3C3F8B, 0xF8F390, "entry 1 atexit"),
    (0x3C4048, 0x3C2A00, "entry 2 helper"),
    (0x3C4066, 0xF8F390, "entry 2 atexit"),
    (0x3C4123, 0x3C2A00, "entry 3 helper"),
    (0x3C4144, 0xF8F390, "entry 3 atexit"),
    (0x3C4204, 0x3C2A00, "entry 4 helper"),
    (0x3C4229, 0xF8F390, "entry 4 atexit"),
    (0x3C42B7, 0x20AF0, "command 0 base"),
    (0x3C431A, 0xF590, "command 0 description view"),
    (0x3C4327, 0xF590, "command 0 name view"),
    (0x3C433D, 0xF8F390, "command 0 atexit"),
    (0x3C471C, 0x20540, "rewind reserve"),
    (0x3C4788, 0xF180, "rewind empty view"),
    (0x3C47B1, 0x20640, "rewind hidden grow/push"),
    (0x3C47C9, 0xF590, "rewind stack view"),
    (0x3C47EF, 0xF590, "rewind description view"),
    (0x3C47FF, 0xF590, "rewind name view"),
    (0x3C4815, 0xF8F390, "rewind atexit"),
    (0x3C4D2E, 0xF8F3A0, "canary fail"),
]

STRINGS = [
    (0xFE6FA2, "RequestAutomationCommands"),
    (0xFE6FBC, "LoadAutomationPlanRemote"),
    (0xFE6FD5, "SetAutomationIndex"),
    (0xFE7183, "wait"),
    (0xFE7188, "Waits the indicated number of seconds. Params: <seconds to wait>"),
    (0xFE71C9, "conCommand"),
    (0xFE71D4, "Execute the indicated console command. Params: <console command text>"),
    (0xFE721A, "print"),
    (0xFE7220, "Prints text to the console. Params <text to print>"),
    (0xFE7253, "waitForUser"),
    (0xFE725F, "Wait until the user performs some task. User signals they are done by firing the console command automate_userdone. Params: None"),
    (0xFE72E0, "waitForResourceLoading"),
    (0xFE72F7, "Wait until the Resource Loader finishes. Params: None"),
    (0xFE732D, "automate_userdone"),
    (0xFE733F, "If the automation system is waiting for the user to do something with waitForUser, this indicates the user is done and that automation can continue. Params: None"),
    (0xFE73E1, "automate"),
    (0xFE73EA, "Puts a command in the current automation plan. Params: <automation command or console command>"),
    (0xFE7449, "automate_go"),
    (0xFE7455, "Starts the current automation. Params: None"),
    (0xFE7481, "automate_pause"),
    (0xFE7490, "Pauses the current automation. Params: None"),
    (0xFE74BC, "automate_rewind"),
    (0xFE74CC, "Rewinds the current automation. Params: None"),
    (0xFE74F9, "automate_clear"),
    (0xFE7508, "Clears the current automation plan. Params: None"),
    (0xFE7539, "automate_loadstring"),
    (0xFE754D, "Loads a string as an automation plan. Params: <string to load>"),
    (0xFE758C, "automate_loadfile"),
    (0xFE759E, "Loads a text file as an automation plan. Params: <name of file to load>"),
    (0xFE75E6, "automate_throwerror"),
    (0xFE75FA, "Produces an automation error for testing. Params: None"),
    (0xFE7631, "writetofile"),
    (0xFE763D, "Writes data to file. Useful for signalling completion. Params: <name of file to write to> <text to write out>"),
    (0xFE7955, "pause"),
]

CONSTS = {
    "kSlot468Rva": BASE,
    "kSlot468Size": SIZE,
    "kSlot468TableWord": 0x2DD6240,
    "kSlot468AppBase": 0x2EF6408,
    "kSlot468EntryBase": 0x2EF64F8,
    "kSlot468CommandBase": 0x2EF67C8,
    "kSlot468AppCount": 3,
    "kSlot468EntryCount": 5,
    "kSlot468CommandCount": 10,
    "kSlot468AppDtor": 0x4B660,
    "kSlot468EntryDtor": 0x3C15E0,
    "kSlot468CommandDtor": 0x4B0,
    "kSlot468CommandVptr": 0x1594DD0,
    "kSlot468CommandVtable": 0x1594DC0,
    "kSlot468CommandRegisterRva": 0x20D60,
    "kSlot468CommandTail8": 0x1990,
    "kSlot468CommandFlags": 2,
    "kSlot468ReserveRva": 0x20540,
    "kSlot468ReserveSize": 253,
    "kSlot468PushRva": 0x20640,
    "kSlot468PushSize": 342,
    "kSlot468EmptyViewRva": 0xF180,
    "kSlot468EmptyViewSize": 28,
    "kSlot468TokenEmptyData": 0x2DD9F78,
    "kSlot468RewindCell": 0x15B0E10,
    "kSlot468RewindSecondaryHook": 0x3C3A80,
}

HEADER_TOKENS = [
    "0x3c3a90", "0x2dd6240", "0x2ef6408", "0x2ef64f8", "0x2ef67c8",
    "0x4b660", "0x3c15e0", "0x1594dd0", "0x20d60", "0x20540", "0x20640", "0xf180",
    "0x2dd9f78", "0x3c3a80", "Slot468Token", "Slot468Command", "identity data",
]
SOURCE_TOKENS = [
    "init_named_delegate_entry", "init_scripted_entry_3c2a00", "init_concmd_base_20af0",
    "register_concmd_20d60", "release_inert_view_f590", "token.flag48 = 1",
    "rewind_grow_path = false", "record_atexit",
]
TEST_TOKENS = [
    "queue.head == &objects.apps[2]", "command.vec_begin == &objects.rewind_token",
    "token.flag48 == 1", "registry.pending.size() == 10", "r.atexit_records == 18",
    "section_registry", "section_failure",
]
CMAKE_TOKENS = ["core/ctor_slot_468_3c3a90.cpp", "test_ctor_slot_468.cpp",
                "core/scripted_entry_3c2a00.cpp"]


def run_elf(elf: Elf) -> None:
    require(elf.u64(TABLE_START - 8 * 468) == BASE, "constructor table word 468")
    elf.body_hash("ctor_slot_468", BASE, SIZE, SHA)
    elf.bytes_at(BASE + SIZE, PAD, "12 nops after slot 468")
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
    for off in (0, 1, 0x20, 0x1000, SIZE - 2):
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
    for rva, _ in STRINGS[:4]:
        elf.overlay = {rva: elf.raw(rva, 1)[0] ^ 0x01}
        ran += 1
        try:
            run_elf(elf)
        except SystemExit:
            continue
        raise SystemExit(f"FAIL string mutant survived at {rva:#x}")
    elf.overlay = {}
    mutated = header.replace("kSlot468Size = 4772;", "kSlot468Size = 4773;", 1)
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
    header = (ROOT / "native/core/ctor_slot_468_3c3a90.hpp").read_text()
    source = (ROOT / "native/core/ctor_slot_468_3c3a90.cpp").read_text()
    test = (ROOT / "native/tests/test_ctor_slot_468.cpp").read_text()
    cmake = (ROOT / "native/CMakeLists.txt").read_text()

    if "--tamper-native" in args:
        mutated = header.replace("kSlot468Size = 4772;", "kSlot468Size = 4773;", 1)
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
        print(f"OK check_ctor_slot_468: 4,772 B hash, {len(CALLEES)} callee hashes, "
              f"{len(RIP_PINS)} object/rip pins, {len(CALL_PINS)} calls, {len(STRINGS)} strings; "
              f"{ran} mutants rejected")
    return 0


if __name__ == "__main__":
    sys.exit(main())
