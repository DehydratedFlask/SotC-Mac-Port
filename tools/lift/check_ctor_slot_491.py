#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 491 (RVA 0x295f90, 372 bytes), lane M5.

Nine zeroed handle cells (0xaaf040 / atexit 0xaaf050), identity vptr 0x159e0f8, caption "<TEXT NOT FOUND>" (atexit 0xf7b0).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and indirect call of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings and the hash of every shared callee. A mutation self-test flips the
first/last byte of every pin and perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lane_m5_check as lib  # noqa: E402

SPEC = {
    "slot": 491,
    "base": 0x295f90,
    "size": 372,
    "body_sha256": "cf351757206bc6324930be124d40e826eb3ae2c734fc42bba941af89d1599798",
    "pad": "909090909090909090909090",
    "callees": ['handle_zero_aaf040', 'handle_dtor_aaf050', 'sharedstr_dtor_f7b0'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0xfc6ac3, b'<TEXT NOT FOUND>'),
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x295f9a, 7, 3, 0x2eef540),
        ("call", 0x295fa4, 0xaaf040),
        ("rip", 0x295fa9, 7, 3, 0xaaf050),
        ("rip", 0x295fb0, 7, 3, 0x1604218),
        ("call", 0x295fc0, 0xf8f390),
        ("rip", 0x295fc5, 7, 3, 0x2eef548),
        ("call", 0x295fcf, 0xaaf040),
        ("call", 0x295fdd, 0xf8f390),
        ("rip", 0x295fe2, 7, 3, 0x2eef550),
        ("call", 0x295fec, 0xaaf040),
        ("call", 0x295ffa, 0xf8f390),
        ("rip", 0x295fff, 7, 3, 0x2eef558),
        ("call", 0x296009, 0xaaf040),
        ("call", 0x296017, 0xf8f390),
        ("rip", 0x29601c, 7, 3, 0x2eef560),
        ("call", 0x296026, 0xaaf040),
        ("call", 0x296034, 0xf8f390),
        ("rip", 0x296039, 7, 3, 0x2eef568),
        ("call", 0x296043, 0xaaf040),
        ("call", 0x296051, 0xf8f390),
        ("rip", 0x296056, 7, 3, 0x2eef570),
        ("call", 0x296060, 0xaaf040),
        ("call", 0x29606e, 0xf8f390),
        ("rip", 0x296073, 7, 3, 0x2eef578),
        ("call", 0x29607d, 0xaaf040),
        ("call", 0x29608b, 0xf8f390),
        ("rip", 0x296090, 7, 3, 0x2eef580),
        ("call", 0x29609a, 0xaaf040),
        ("call", 0x2960a8, 0xf8f390),
        ("rip", 0x2960ad, 7, 3, 0x159e0e8),
        ("rip", 0x2960b4, 7, 3, 0xf7b0),
        ("rip", 0x2960bb, 7, 3, 0xfc6ac3),
        ("rip", 0x2960c6, 7, 3, 0x2eef5a0),
        ("rip", 0x2960d0, 7, 3, 0x2eef5a0),
        ("rip", 0x2960d7, 11, 3, 0x2eef5a8),
        ("rip", 0x2960e2, 8, 4, 0x2eef5b0),
        ("imm", 0x2960ea, "4883c010"),
        ("rip", 0x2960ee, 7, 3, 0x2eef588),
        ("imm", 0x2960f5, "4883c408"),
        ("jmp", 0x2960ff, 0xf8f390),
    ],
    "elf_extra": [],
    "native": {'header': 'native/core/ctor_slot_491_295f90.hpp',
     'source': 'native/core/ctor_slot_491_295f90.cpp',
     'test': 'native/tests/test_ctor_slot_491.cpp',
     'cmake': 'native/lanes/m5.cmake',
     'consts': {'kSlot491Rva': 2711440,
                'kSlot491Size': 372,
                'kSlot491FirstHandleRva': 49214784,
                'kSlot491IdentityCellRva': 49214856,
                'kSlot491IdentityVptr': 22667512,
                'kSlot491CaptionRva': 49214880,
                'kSlot491HandleDtorRva': 11202640,
                'kSlot491CaptionDtorRva': 63408},
     'header_tokens': [],
     'source_order': ['handle = nullptr;',
                      'cleanup.record(kSlot491HandleDtorRva, &handle)',
                      'objects.caption.data = ',
                      'objects.caption.size = ',
                      'objects.identity_vptr = kSlot491IdentityVptr',
                      'cleanup.record(kSlot491CaptionDtorRva, &objects.caption)'],
     'test_tokens': ['kSlot491Rva == 0x295f90', 'section_alone()', 'section_idempotent_state()'],
     'cmake_tokens': ['core/ctor_slot_491_295f90.cpp',
                      'add_executable(test_ctor_slot_491 tests/test_ctor_slot_491.cpp)',
                      'add_test(NAME ctor_slot_491 COMMAND test_ctor_slot_491)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Nine zeroed handle cells (0xaaf040 / atexit 0xaaf050), identity vptr 0x159e0f8, caption "<TEXT NOT FOUND>" (atexit 0xf7b0).'))
