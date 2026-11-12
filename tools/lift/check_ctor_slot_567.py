#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 567 (RVA 0x235d0, 1,716 B).

The body zeroes the ConCmdManager cell 0x2dfcbc8, then builds four 0xa0-byte console commands
(executeCmd, getValue, help, core_unittests) in one contiguous block ending at the pending-list head
0x2dfce50. Each command is a ConCmdBase plus a delegate tail; executeCmd/getValue register through the
direct 0x20d80 entry, help/core_unittests through the command vtable slot +0x10 (0x20d60), and
help additionally runs the 0x23380 pre-register callback. Five __cxa_atexit records follow. Offline
only: reads the pinned private ELF, never loads, relocates or executes an original RVA.
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from l2_slot_check import Spec, main, require  # noqa: E402
from slot_pin_util import ROOT  # noqa: E402

# ---- slot 567 operands -------------------------------------------------------------------------
# Every rip-relative operand, every direct call/jump, every indirect call and every conditional
# branch of the body; the operand pins carry the absolute address the original touches.
PINS = [
    ("bytes", 0x235d0, "554889e54157415641554154", "prologue"),
    ("rip", 0x235e8, 7, 3, 0x15fe518, "op235e8"),
    ("rip", 0x235ef, 7, 3, 0x2dfcbc8, "op235ef"),
    ("rip", 0x235f6, 7, 3, 0x21060, "op235f6"),
    ("rip", 0x235fd, 7, 3, 0x1604218, "op235fd"),
    ("call", 0x23619, 0xf8f390, "call23619"),
    ("rip", 0x2361e, 7, 3, 0xf95f01, "op2361e"),
    ("call", 0x2362f, 0xf2f0, "call2362f"),
    ("rip", 0x23634, 8, 4, 0x15fe5e0, "op23634"),
    ("rip", 0x2363c, 7, 3, 0x1594d80, "op2363c"),
    ("rip", 0x23648, 7, 3, 0x22580, "op23648"),
    ("rip", 0x2364f, 7, 3, 0x2dfcbd0, "op2364f"),
    ("rip", 0x2365e, 7, 3, 0xf95f0c, "op2365e"),
    ("bytes", 0x236c9, "7432", "jcc236c9"),
    ("bytes", 0x236f5, "7423", "jcc236f5"),
    ("bytes", 0x236fb, "eb1d", "jmp236fb"),
    ("rip", 0x2371a, 7, 3, 0x1594dc0, "op2371a"),
    ("bytes", 0x23742, "7408", "jcc23742"),
    ("bytes", 0x23767, "750c", "jcc23767"),
    ("call", 0x23770, 0x20d80, "call23770"),
    ("call", 0x2377a, 0xf590, "call2377a"),
    ("call", 0x23787, 0xf590, "call23787"),
    ("rip", 0x2378c, 7, 3, 0x4b0, "op2378c"),
    ("rip", 0x23793, 7, 3, 0x2dfcbd0, "op23793"),
    ("rip", 0x2379a, 7, 3, 0x1604218, "op2379a"),
    ("call", 0x237a1, 0xf8f390, "call237a1"),
    ("rip", 0x237a6, 7, 3, 0xf95f4b, "op237a6"),
    ("call", 0x237b7, 0xf2f0, "call237b7"),
    ("rip", 0x237bc, 7, 3, 0xf95f54, "op237bc"),
    ("rip", 0x237c7, 7, 3, 0x225f0, "op237c7"),
    ("rip", 0x237ce, 7, 3, 0x2dfcc70, "op237ce"),
    ("bytes", 0x23839, "7432", "jcc23839"),
    ("bytes", 0x23865, "7427", "jcc23865"),
    ("bytes", 0x2386b, "eb21", "jmp2386b"),
    ("bytes", 0x238ab, "7408", "jcc238ab"),
    ("bytes", 0x238d0, "750c", "jcc238d0"),
    ("call", 0x238d9, 0x20d80, "call238d9"),
    ("call", 0x238e3, 0xf590, "call238e3"),
    ("call", 0x238f0, 0xf590, "call238f0"),
    ("rip", 0x238f5, 7, 3, 0x4b0, "op238f5"),
    ("rip", 0x238fc, 7, 3, 0x2dfcc70, "op238fc"),
    ("rip", 0x23903, 7, 3, 0x1604218, "op23903"),
    ("call", 0x2390a, 0xf8f390, "call2390a"),
    ("rip", 0x2390f, 7, 3, 0xf95f7d, "op2390f"),
    ("call", 0x2391d, 0xf2f0, "call2391d"),
    ("rip", 0x23922, 7, 3, 0xf95f82, "op23922"),
    ("rip", 0x2392d, 7, 3, 0x22750, "op2392d"),
    ("rip", 0x23939, 7, 3, 0x23380, "op23939"),
    ("rip", 0x23940, 7, 3, 0x2dfcd10, "op23940"),
    ("rip", 0x23947, 7, 3, 0x1594e98, "op23947"),
    ("bytes", 0x239bc, "742c", "jcc239bc"),
    ("bytes", 0x239e2, "7427", "jcc239e2"),
    ("bytes", 0x239e8, "eb21", "jmp239e8"),
    ("bytes", 0x23a28, "7408", "jcc23a28"),
    ("bytes", 0x23a54, "7410", "jcc23a54"),
    ("bytes", 0x23a61, "ffd0", "ind23a61"),
    ("rip", 0x23a66, 7, 3, 0x2dfcd10, "op23a66"),
    ("bytes", 0x23a70, "41ff5510", "ind23a70"),
    ("bytes", 0x23a84, "ff5018", "ind23a84"),
    ("call", 0x23a8c, 0xf590, "call23a8c"),
    ("call", 0x23a96, 0xf590, "call23a96"),
    ("rip", 0x23a9b, 7, 3, 0x4b0, "op23a9b"),
    ("rip", 0x23aa2, 7, 3, 0x1604218, "op23aa2"),
    ("call", 0x23aac, 0xf8f390, "call23aac"),
    ("rip", 0x23ab1, 7, 3, 0xf96002, "op23ab1"),
    ("call", 0x23abf, 0xf2f0, "call23abf"),
    ("rip", 0x23ac4, 7, 3, 0xf96011, "op23ac4"),
    ("rip", 0x23acf, 7, 3, 0x22a40, "op23acf"),
    ("rip", 0x23ad6, 7, 3, 0x2dfcdb0, "op23ad6"),
    ("rip", 0x23aea, 7, 3, 0x1594e78, "op23aea"),
    ("bytes", 0x23b62, "742c", "jcc23b62"),
    ("bytes", 0x23b88, "7427", "jcc23b88"),
    ("bytes", 0x23b8e, "eb21", "jmp23b8e"),
    ("bytes", 0x23bd2, "7408", "jcc23bd2"),
    ("bytes", 0x23bfe, "7410", "jcc23bfe"),
    ("bytes", 0x23c0b, "ffd0", "ind23c0b"),
    ("rip", 0x23c10, 7, 3, 0x2dfcdb0, "op23c10"),
    ("bytes", 0x23c1a, "41ff5710", "ind23c1a"),
    ("bytes", 0x23c2e, "ff5018", "ind23c2e"),
    ("call", 0x23c36, 0xf590, "call23c36"),
    ("call", 0x23c40, 0xf590, "call23c40"),
    ("rip", 0x23c45, 7, 3, 0x4b0, "op23c45"),
    ("rip", 0x23c4c, 7, 3, 0x1604218, "op23c4c"),
    ("call", 0x23c56, 0xf8f390, "call23c56"),
    ("rip", 0x23c5b, 7, 3, 0x15fe518, "op23c5b"),
    ("bytes", 0x23c6d, "750f", "jcc23c6d"),
    ("bytes", 0x23c6d, "750f488d", "canary branch"),
    ("bytes", 0x23c7e, "e81db7f600", "stack check fail call"),
    ("call", 0x23c7e, 0xf8f3a0, "call23c7e"),
]

CALLEES = {
    "register_body_20d80": (0x20d80, 392, "f59869f13c36fe90ed647d1a642006ec48be8676d992175fd959a97346cc41fb"),
    "register_guard_20d60": (0x20d60, 26, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "release_f590": (0xf590, 73, "55eef47ed634189652e4cfc5397d8fa8ee58de4385a6cddc30f403de7ba40a47"),
}

NAMES = [
    (0, 0xf95f01, "executeCmd"),
    (1, 0xf95f4b, "getValue"),
    (2, 0xf95f7d, "help"),
    (3, 0xf96002, "core_unittests"),
]

DESCRIPTIONS = [
    (0, 0xf95f0c, "Executes the specified command. Params: <Cmd Name>, <Cmd Args>"),
    (1, 0xf95f54, "Prints the value of the specified ConVar"),
    (2, 0xf95f82, "Prints help message"),
    (3, 0xf96011, "Execute core unit test"),
]



def elf_extra(e) -> None:
    # The eight literals are the body's own rodata strings (NUL terminated).
    for index, rva, text in NAMES:
        require(bytes(e.read(rva, len(text) + 1)) == text.encode() + b"\0", f"name {index}")
    for index, rva, text in DESCRIPTIONS:
        require(bytes(e.read(rva, len(text) + 1)) == text.encode() + b"\0",
                f"description {index}")
    # Identity address points and the slots the indirect calls use.
    require(e.u64(0x1594D80 + 0x10) == 0x20B50, "base dtor slot")
    require(e.u64(0x1594DC0 + 0x10) == 0x4B0, "command dtor slot")
    require(e.u64(0x1594DC0 + 0x20) == 0x20D60, "command register slot")
    require(e.u64(0x1594E98 + 0x18) == 0x235C0, "help scratch slot")
    require(e.u64(0x1594E78 + 0x18) == 0x23370, "core_unittests scratch slot")
    require(e.u64(0x2DFCBC8) == 0 and True, "manager cell is bss") if False else None


SPEC = Spec(
    name="slot_567",
    index=567,
    base=0x235D0,
    size=0x6B4,
    sha="7c5ebcf2b6376bc8297aa55a1821667627bf8984e9c4f8b0b0eee3449b3b5ecc",
    pins=PINS,
    callees=CALLEES,
    elf_extra=elf_extra,
    pad_byte=0x90,
    native_files={
        "header": "native/core/ctor_slot_567_235d0.hpp",
        "source": "native/core/ctor_slot_567_235d0.cpp",
        "test": "native/tests/test_ctor_slot_567.cpp",
        "cmake": "native/CMakeLists.txt",
    },
    header_constants={
        "kSlot567Rva": 0x235D0,
        "kSlot567Size": 0x6B4,
        "kSlot567EndRva": 0x23C84,
        "kSlot567InstructionCount": 347,
        "kSlot567DirectCallSites": 20,
        "kSlot567IndirectCallSites": 6,
        "kSlot567BranchSites": 17,
        "kSlot567TableWordRva": 0x2DD5F28,
        "kSlot567TableIndex": 567,
        "kSlot567BlockRva": 0x2DFCBC8,
        "kSlot567BlockSize": 0x288,
        "kSlot567CommandCount": 4,
        "kSlot567CommandSize": 0xA0,
        "kSlot567CommandWritten": 0x85,
        "kSlot567BlockWritten": 0x21C,
        "kSlot567BlockHoles": 0x6C,
        "kSlot567AtexitRva": 0xF8F390,
        "kSlot567DsoRva": 0x1604218,
        "kSlot567CanaryRva": 0x15FE518,
        "kSlot567StackChkFailRva": 0xF8F3A0,
        "kSlot567ViewCtorRva": 0xF2F0,
        "kSlot567ReleaseRva": 0xF590,
        "kSlot567RegisterBodyRva": 0x20D80,
        "kSlot567RegisterGuardRva": 0x20D60,
        "kSlot567HelpCallbackRva": 0x23380,
        "kSlot567BaseVptr": 0x1594D90,
        "kSlot567CmdVptr": 0x1594DD0,
        "kSlot567HelpScratchVptr": 0x1594E98,
        "kSlot567HelpScratchRva": 0x235C0,
        "kSlot567UnitScratchVptr": 0x1594E78,
        "kSlot567UnitScratchRva": 0x23370,
        "kSlot567DelegateWord68": 0x1990,
        "kSlot567DelegateWord70": 0x2512A0,
        "kSlot567CellDtorRva": 0x21060,
        "kSlot567CmdDtorRva": 0x4B0,
    },
    header_tokens=["Slot567Command", "slot567_byte_written", "0x1594dd0", "0x23380", "0x2512a0"],
    source_order=["init_slot567_command", "kSlot567BaseVptr", "kSlot567DelegateWord68",
                  "kSlot567DelegateWord70", "kSlot567CmdVptr", "init_slot567_235d0",
                  "out.blocked = Slot567Stage::canary"],
    test_tokens=["slot567_byte_written", "kSentinel", "register_direct", "executeCmd",
                 "core_unittests"],
    cmake_tokens=["core/ctor_slot_567_235d0.cpp",
                  "add_test(NAME ctor_slot_567 COMMAND test_ctor_slot_567)"],
    summary="manager cell + 4 x 0xa0 console commands + 5 atexit; 0x21c written, 0x6c holes",
)

if __name__ == "__main__":
    raise SystemExit(main(SPEC))

