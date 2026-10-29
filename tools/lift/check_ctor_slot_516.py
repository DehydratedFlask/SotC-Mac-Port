#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 516 (RVA 0x1b97b0, 177 bytes), lane M5.

String variable `network_PlayerName` (empty initial value) built by 0x257a0, init view released before name view, atexit 0x23c90.

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
    "slot": 516,
    "base": 0x1b97b0,
    "size": 177,
    "body_sha256": "3aec1bbfaaea93798d0244cd09e830023b02c39379248ce118c351df4f1c3fd1",
    "pad": "909090909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'release_f590', 'string_ctor', 'string_inner', 'string_dtor_23c90', 'empty_ctor_f180', 'sfn_copy', 'sfn_clone', 'sfn_move', 'sfn_destroy', 'str_slot3', 'str_slot6', 'str_dtor_mid'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0xfaf050, b'network_PlayerName'),
        (0xfaf063, b''),
    ],
    "lengths": [
        ('network_PlayerName', 0x1b97fc, "48c745c012000000"),
    ],
    "pins": [
        ("rip", 0x1b97bf, 7, 3, 0x15fe518),
        ("rip", 0x1b97c6, 7, 3, 0x2ee6780),
        ("rip", 0x1b97d1, 7, 3, 0xfaf050),
        ("rip", 0x1b97e0, 7, 3, 0xfaf063),
        ("imm", 0x1b97fc, "48c745c012000000"),
        ("imm", 0x1b9812, "48c745b000000000"),
        ("call", 0x1b981a, 0x257a0),
        ("call", 0x1b9822, 0xf590),
        ("call", 0x1b982a, 0xf590),
        ("rip", 0x1b982f, 7, 3, 0x23c90),
        ("rip", 0x1b9836, 7, 3, 0x1604218),
        ("call", 0x1b9840, 0xf8f390),
        ("imm", 0x1b984f, "4883c450"),
        ("call", 0x1b985c, 0xf8f3a0),
    ],
    "elf_extra": [lib.string_var_extra],
    "native": {'header': 'native/core/ctor_slot_516_1b97b0.hpp',
     'source': 'native/core/ctor_slot_516_1b97b0.cpp',
     'test': 'native/tests/test_ctor_slot_516.cpp',
     'cmake': 'native/lanes/m5.cmake',
     'consts': {'kSlot516Rva': 1808304, 'kSlot516Size': 177},
     'header_tokens': ['kSlot516ObjectRva = 0x2ee6780'],
     'source_order': ['init_string_var_257a0(objects.var, name, init, concmd)',
                      'release_inert_view_f590(init)',
                      'release_inert_view_f590(name)',
                      'concmd.atexit_log.record(kStringVarDtorRva, &objects.var)'],
     'test_tokens': ['kSlot516Rva == 0x1b97b0', 'section_alone()', 'section_failures()'],
     'cmake_tokens': ['core/ctor_slot_516_1b97b0.cpp',
                      'add_executable(test_ctor_slot_516 tests/test_ctor_slot_516.cpp)',
                      'add_test(NAME ctor_slot_516 COMMAND test_ctor_slot_516)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'String variable `network_PlayerName` (empty initial value) built by 0x257a0, init view released before name view, atexit 0x23c90.'))
