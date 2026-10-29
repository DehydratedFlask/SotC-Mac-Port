#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 559 (RVA 0x3f2d0, 456 bytes), lane M6.

Three inline small vectors (0x1158 B each, object at 0x2ea5170), __cxa_atexit(0x3d920, NULL) and the bool console
variable `loggingIncludeTimestamp` (functor tag 0x1595770, value 0 / default 0, empty description, atexit 0x20b0).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every instruction operand of the
body (rip-relative targets, call/jmp targets, immediates, indirect calls; generated once from the
disassembly, re-verified here from bytes), the referenced strings, the functor vtables and the hash of
every shared callee / destructor. A mutation self-test flips the first/last byte of every pin and
perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lane_m6_lib as lib  # noqa: E402

SPEC = {
    "slot": 559,
    "base": 0x3f2d0,
    "size": 456,
    "body_sha256": "64eab3fb87742597e9795f826f416d9db6dfcb645707a50d4283c2e4ac89695b",
    "pad": "9090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590'],
    "bool_tags": [0x1595770],
    "float_tags": [],
    "strings": [
        (0xf9858c, b'loggingIncludeTimestamp'),
        (0xf98920, b''),
    ],
    "lengths": [
        ('loggingIncludeTimestamp', 0x3f3c2, "48c7459817000000"),
    ],
    "pins": [
        ("imm", 0x3f2d1, "4889e5"),
        ("imm", 0x3f2dd, "4883ec48"),
        ("rip", 0x3f2e1, 7, 3, 0x15fe518),
        ("rip", 0x3f2e8, 7, 3, 0x1604218),
        ("imm", 0x3f2ef, "31f6"),
        ("imm", 0x3f2f1, "488b00"),
        ("imm", 0x3f2f4, "488945d0"),
        ("rip", 0x3f2f8, 7, 3, 0x2ea5170),
        ("imm", 0x3f2ff, "488d4828"),
        ("imm", 0x3f303, "488db880110000"),
        ("imm", 0x3f30a, "488d90d8220000"),
        ("imm", 0x3f311, "c4e1f96ec1"),
        ("imm", 0x3f316, "48894820"),
        ("imm", 0x3f31a, "488d8858110000"),
        ("imm", 0x3f321, "c5f970c044"),
        ("imm", 0x3f326, "c5fa7f00"),
        ("imm", 0x3f32a, "c4e1f96ec7"),
        ("imm", 0x3f32f, "48894810"),
        ("imm", 0x3f333, "4889b878110000"),
        ("imm", 0x3f33a, "488d88b0220000"),
        ("rip", 0x3f341, 7, 3, 0x3d920),
        ("imm", 0x3f348, "c5f970c044"),
        ("imm", 0x3f34d, "c5fa7f8058110000"),
        ("imm", 0x3f355, "c4e1f96ec2"),
        ("imm", 0x3f35a, "48898868110000"),
        ("imm", 0x3f361, "488990d0220000"),
        ("imm", 0x3f368, "4c89f2"),
        ("imm", 0x3f36b, "488d8808340000"),
        ("imm", 0x3f372, "c5f970c044"),
        ("imm", 0x3f377, "c5fa7f80b0220000"),
        ("imm", 0x3f37f, "488988c0220000"),
        ("call", 0x3f386, 0xf8f390),
        ("rip", 0x3f38b, 7, 3, 0x1594208),
        ("rip", 0x3f392, 7, 3, 0x2ea8578),
        ("rip", 0x3f399, 7, 3, 0xf9858c),
        ("imm", 0x3f3a0, "4c8d7d90"),
        ("imm", 0x3f3a4, "4c8d65b0"),
        ("imm", 0x3f3a8, "c5f9efc0"),
        ("imm", 0x3f3ac, "b901000000"),
        ("imm", 0x3f3b1, "48894590"),
        ("rip", 0x3f3b5, 7, 3, 0xf98920),
        ("imm", 0x3f3bc, "4c89fe"),
        ("imm", 0x3f3bf, "4c89e2"),
        ("imm", 0x3f3c2, "48c7459817000000"),
        ("imm", 0x3f3ca, "c5fa7f45a0"),
        ("imm", 0x3f3cf, "488945b0"),
        ("imm", 0x3f3d3, "c5fa7f45b8"),
        ("imm", 0x3f3d8, "48c745c800000000"),
        ("imm", 0x3f3e0, "4983c510"),
        ("imm", 0x3f3e4, "4889df"),
        ("imm", 0x3f3e7, "4c892b"),
        ("call", 0x3f3ea, 0x20af0),
        ("rip", 0x3f3ef, 7, 3, 0x15940a0),
        ("imm", 0x3f3f6, "4c89e7"),
        ("imm", 0x3f3f9, "488d93c0000000"),
        ("imm", 0x3f400, "c5fc57c0"),
        ("rip", 0x3f404, 7, 3, 0x1595770),
        ("imm", 0x3f40b, "4883c010"),
        ("imm", 0x3f40f, "488903"),
        ("imm", 0x3f412, "48895360"),
        ("imm", 0x3f416, "c7436900000000"),
        ("imm", 0x3f41d, "c5fc114370"),
        ("imm", 0x3f422, "48c783a000000000000000"),
        ("imm", 0x3f42d, "48898bb0000000"),
        ("imm", 0x3f434, "48c783b800000000000000"),
        ("call", 0x3f43f, 0xf590),
        ("imm", 0x3f444, "4889df"),
        ("imm", 0x3f447, "4c892b"),
        ("imm", 0x3f44a, "c683c000000000"),
        ("imm", 0x3f451, "c6436800"),
        ("call", 0x3f455, 0x20d60),
        ("imm", 0x3f45a, "4c89ff"),
        ("call", 0x3f45d, 0xf590),
        ("rip", 0x3f462, 7, 3, 0x20b0),
        ("imm", 0x3f469, "4889de"),
        ("imm", 0x3f46c, "4c89f2"),
        ("call", 0x3f46f, 0xf8f390),
        ("rip", 0x3f474, 7, 3, 0x15fe518),
        ("imm", 0x3f47b, "488b00"),
        ("imm", 0x3f47e, "483b45d0"),
        ("imm", 0x3f482, "750f"),
        ("imm", 0x3f484, "4883c448"),
        ("call", 0x3f493, 0xf8f3a0),
    ],
    "m6_callees": {
        "object_dtor_3d920": (0x3d920, 0xff, "f36ffaa76493bbc7fd238783af77a73371e9c8f7b064814932c3ccdd1ed43487"),
    },
    "native": {
        'header': 'native/core/ctor_slot_559_3f2d0.hpp',
        'source': 'native/core/ctor_slot_559_3f2d0.cpp',
        'test': 'native/tests/test_ctor_slot_559.cpp',
        'cmake': 'native/lanes/m6.cmake',
        'consts': {'kSlot559Rva': 258768, 'kSlot559Size': 456, 'kSlot559ObjectDtorRva': 252192, 'kSlot559BoolFnTag': 22632304, 'kSlot559VarNameLength': 23, 'kSlot559VectorBytes': 4440, 'kSlot559VectorStorageBytes': 4400},
        'header_tokens': ['offsetof(Slot559Vector, storage) == 0x28'],
        'source_order': ['v.begin = v.storage;', 'v.end = v.storage;', 'v.inline_begin = v.storage;', 'registry.atexit_log.record(kSlot559ObjectDtorRva, nullptr)', 'lane_m6::init_bool_var_literal(objects.var, kSlot559VarName, kSlot559VarNameLength, "", 0,', 'kSlot559BoolFnTag, registry, 0, 0)'],
        'test_tokens': ['kSlot559Rva == 0x3f2d0', 'section_alone()', 'section_failures()', 'check_atexit(creg, 0, 0x3d920, nullptr)'],
        'cmake_tokens': ['core/ctor_slot_559_3f2d0.cpp', 'add_executable(test_ctor_slot_559 tests/test_ctor_slot_559.cpp)', 'add_test(NAME ctor_slot_559 COMMAND test_ctor_slot_559)'],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, '3 inline vectors + atexit(0x3d920, null) + bool var loggingIncludeTimestamp (0/0, tag 0x1595770)'))
