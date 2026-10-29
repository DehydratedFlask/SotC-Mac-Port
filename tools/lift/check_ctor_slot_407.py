#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 407 (RVA 0x5d41c0, 374 bytes), lane M2.

Bool variable `cp10_creditsUseNewList` (functor tag 0x15cb258, value/default 1, flags 1), a 12-byte zero vec3 (0x2ff649c) and a 64-byte 4x4 float identity matrix (0x2ff64ac), no atexit for the latter two.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and register store of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings, the functor vtables and the hash of every shared callee. A mutation
self-test flips the first/last byte of every pin and perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

SPEC = {
    "slot": 407,
    "base": 0x5d41c0,
    "size": 374,
    "body_sha256": "c0e56e7d4e9bc5c4cf87bea00e2582d809f228b869fa18e8e79d1559fbab96f5",
    "pad": "90909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15cb258],
    "float_tags": [],
    "strings": [
        (0x1268590, b'cp10_creditsUseNewList'),
        (0x12686ab, b''),
    ],
    "lengths": [
        ('cp10_creditsUseNewList', 0x5d4222, "48c7459816000000"),
    ],
    "pins": [
        ("rip", 0x5d41d1, 7, 3, 0x15fe518),
        ("rip", 0x5d41d8, 7, 3, 0x1594208),
        ("rip", 0x5d41df, 7, 3, 0x2ff59c0),
        ("rip", 0x5d41e6, 7, 3, 0x1268590),
        ("rip", 0x5d41ed, 7, 3, 0x12686ab),
        ("imm", 0x5d41fc, "c5f857c0"),
        ("imm", 0x5d4200, "b901000000"),
        ("imm", 0x5d4209, "4983c510"),
        ("imm", 0x5d4222, "48c7459816000000"),
        ("imm", 0x5d4234, "4c892b"),
        ("imm", 0x5d4237, "48c745c800000000"),
        ("call", 0x5d423f, 0x20af0),
        ("rip", 0x5d4244, 7, 3, 0x15940a0),
        ("imm", 0x5d424e, "488d93c0000000"),
        ("rip", 0x5d4259, 7, 3, 0x15cb258),
        ("imm", 0x5d4260, "4883c010"),
        ("imm", 0x5d4264, "488903"),
        ("imm", 0x5d4267, "48895360"),
        ("imm", 0x5d426b, "c7436900000000"),
        ("imm", 0x5d4272, "c5fc114370"),
        ("imm", 0x5d4277, "48c783a000000000000000"),
        ("imm", 0x5d4282, "48898bb0000000"),
        ("imm", 0x5d4289, "48c783b800000000000000"),
        ("call", 0x5d4294, 0xf590),
        ("imm", 0x5d429c, "4c892b"),
        ("imm", 0x5d429f, "c683c000000001"),
        ("imm", 0x5d42a6, "c6436801"),
        ("call", 0x5d42aa, 0x20d60),
        ("call", 0x5d42b2, 0xf590),
        ("rip", 0x5d42b7, 7, 3, 0x20b0),
        ("rip", 0x5d42be, 7, 3, 0x1604218),
        ("call", 0x5d42c8, 0xf8f390),
        ("rip", 0x5d42cd, 7, 3, 0x2ff64ac),
        ("rip", 0x5d42d4, 7, 3, 0x2ff649c),
        ("imm", 0x5d42db, "c5f857c0"),
        ("imm", 0x5d42df, "c5f81300"),
        ("imm", 0x5d42e3, "c7010000803f"),
        ("imm", 0x5d42e9, "c5f8114104"),
        ("imm", 0x5d42ee, "c741140000803f"),
        ("imm", 0x5d42f5, "c5f8114118"),
        ("imm", 0x5d42fa, "c5f857c0"),
        ("imm", 0x5d42fe, "c741280000803f"),
        ("imm", 0x5d4305, "c7400800000000"),
        ("imm", 0x5d430c, "c5f811412c"),
        ("imm", 0x5d4311, "c7413c0000803f"),
        ("call", 0x5d4331, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_407_5d41c0.hpp',
     'source': 'native/core/ctor_slot_407_5d41c0.cpp',
     'test': 'native/tests/test_ctor_slot_407.cpp',
     'cmake': 'native/lanes/m2.cmake',
     'consts': {'kSlot407Rva': 6111680, 'kSlot407Size': 374},
     'header_tokens': ['kSlot407FnTag = 0x15cb258', 'kSlot407One = 0x3f800000u'],
     'source_order': ['init_bool_var_site(objects.var, kSlot407VarName, kSlot407FnTag, concmd, 1, 1)',
                      'v[0] = 0;',
                      'm[0] = kSlot407One',
                      'm[5] = kSlot407One',
                      'm[10] = kSlot407One',
                      'v[2] = 0;',
                      'm[15] = kSlot407One'],
     'test_tokens': ['kSlot407Rva == 0x5d41c0', 'section_alone()', 'section_failures()'],
     'cmake_tokens': ['core/ctor_slot_407_5d41c0.cpp',
                      'add_executable(test_ctor_slot_407 tests/test_ctor_slot_407.cpp)',
                      'add_test(NAME ctor_slot_407 COMMAND test_ctor_slot_407)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bool variable `cp10_creditsUseNewList` (tag 0x15cb258) + zero vec3 + 4x4 identity matrix.'))
