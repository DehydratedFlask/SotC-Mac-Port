#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 459 (RVA 0x43f460, 299 bytes), lane m3.

One bool console variable `SM_printAnimPhase` (object 0x2ef7f30, value 0, default 0, functor tag 0x15b70d0). Same stores as slot 348, rip-relative encoding.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and indirect call of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings, the functor vtables and the hash of every shared callee. A mutation
self-test flips the first/last byte of every pin and perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

SPEC = {
    "slot": 459,
    "base": 0x43f460,
    "size": 299,
    "body_sha256": "dbf05b1d1fc7b378817d4fc48f79e2883e59f1e9a1a069aaf4239504c9a48260",
    "pad": "9090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15b70d0],
    "float_tags": [],
    "strings": [
        (0xff2d9c, b'SM_printAnimPhase'),
        (0xff2dda, b''),
    ],
    "lengths": [
        ('SM_printAnimPhase', 0x43f4c2, "48c7459811000000"),
    ],
    "pins": [
        ("rip", 0x43f471, 7, 3, 0x15fe518),
        ("rip", 0x43f478, 7, 3, 0x1594208),
        ("rip", 0x43f47f, 7, 3, 0x2ef7f30),
        ("rip", 0x43f486, 7, 3, 0xff2d9c),
        ("rip", 0x43f48d, 7, 3, 0xff2dda),
        ("imm", 0x43f4a0, "b901000000"),
        ("imm", 0x43f4c2, "48c7459811000000"),
        ("imm", 0x43f4d7, "48c745c800000000"),
        ("call", 0x43f4df, 0x20af0),
        ("rip", 0x43f4e4, 7, 3, 0x15940a0),
        ("rip", 0x43f4f9, 7, 3, 0x15b70d0),
        ("imm", 0x43f50b, "c7436900000000"),
        ("imm", 0x43f517, "48c783a000000000000000"),
        ("imm", 0x43f529, "48c783b800000000000000"),
        ("call", 0x43f534, 0xf590),
        ("imm", 0x43f53f, "c683c000000000"),
        ("imm", 0x43f546, "c6436800"),
        ("call", 0x43f54a, 0x20d60),
        ("call", 0x43f552, 0xf590),
        ("rip", 0x43f557, 7, 3, 0x20b0),
        ("rip", 0x43f55e, 7, 3, 0x1604218),
        ("call", 0x43f568, 0xf8f390),
        ("call", 0x43f586, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_459_43f460.hpp', 'source': 'native/core/ctor_slot_459_43f460.cpp', 'test': 'native/tests/test_ctor_slot_459.cpp', 'cmake': 'native/lanes/m3.cmake', 'consts': {'kSlot459Rva': 4453472, 'kSlot459Size': 299}, 'header_tokens': ['kSlot459FnTag = 0x15b70d0', 'kSlot459ObjectRva = 0x2ef7f30'], 'source_order': ['init_bool_var_site(objects.var, kSlot459Name, kSlot459FnTag, concmd, 0, 0)'], 'test_tokens': ['kSlot459Rva == 0x43f460', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_459_43f460.cpp', 'add_executable(test_ctor_slot_459 tests/test_ctor_slot_459.cpp)', 'add_test(NAME ctor_slot_459 COMMAND test_ctor_slot_459)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'One bool console variable `SM_printAnimPhase` (value 0, default 0, functor tag 0x15b70d0, flags 1): 0x20af0, f590(desc), 0x20d60, f590(name), atexit(0x20b0).'))
