#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 365 (RVA 0x747870, 299 bytes), lane M1.

Single bool console variable `nico_freezeBoss` (object 0x32524c8, value 0/0, functor tag 0x15cd220).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and indirect call of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings, the functor vtables and the hash of every shared callee. A mutation
self-test flips the first/last byte of every pin and perturbs every native token; each mutant must be rejected.
Uses the declarative driver tools/lift/l4_slot_lib.py unchanged.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

SPEC = {
    "slot": 365,
    "base": 0x747870,
    "size": 299,
    "body_sha256": "b41c6bb22d7a4586773e31a55e1f662cc863ec51b0001622da02febb45a1d8fe",
    "pad": "9090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [22860320],
    "float_tags": [],
    "strings": [
        (0x127f788, b'nico_freezeBoss'),
        (0x127f89c, b''),
    ],
    "lengths": [
        ('nico_freezeBoss', 0x7478d2, "48c745980f000000"),
    ],
    "pins": [
        ("rip", 0x747881, 7, 3, 0x15fe518),
        ("rip", 0x747888, 7, 3, 0x1594208),
        ("rip", 0x74788f, 7, 3, 0x32524c8),
        ("rip", 0x747896, 7, 3, 0x127f788),
        ("rip", 0x74789d, 7, 3, 0x127f89c),
        ("imm", 0x7478b0, "b901000000"),
        ("imm", 0x7478d2, "48c745980f000000"),
        ("imm", 0x7478e7, "48c745c800000000"),
        ("call", 0x7478ef, 0x20af0),
        ("rip", 0x7478f4, 7, 3, 0x15940a0),
        ("rip", 0x747909, 7, 3, 0x15cd220),
        ("imm", 0x74791b, "c7436900000000"),
        ("imm", 0x747927, "48c783a000000000000000"),
        ("imm", 0x747939, "48c783b800000000000000"),
        ("call", 0x747944, 0xf590),
        ("imm", 0x74794f, "c683c000000000"),
        ("imm", 0x747956, "c6436800"),
        ("call", 0x74795a, 0x20d60),
        ("call", 0x747962, 0xf590),
        ("rip", 0x747967, 7, 3, 0x20b0),
        ("rip", 0x74796e, 7, 3, 0x1604218),
        ("call", 0x747978, 0xf8f390),
        ("call", 0x747996, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_365_747870.hpp', 'source': 'native/core/ctor_slot_365_747870.cpp', 'test': 'native/tests/test_ctor_slot_365.cpp', 'cmake': 'native/lanes/m1.cmake', 'consts': {'kSlot365Rva': 7633008, 'kSlot365Size': 299}, 'header_tokens': ['kSlot365FnTag = 0x15cd220', 'kSlot365ObjectRva = 0x32524c8'], 'source_order': ['init_bool_var_site(objects.var, kSlot365Name, kSlot365FnTag, concmd, 0, 0)'], 'test_tokens': ['kSlot365Rva == 0x747870', 'section_alone()', 'section_failures()', 'section_with_neighbour()'], 'cmake_tokens': ['core/ctor_slot_365_747870.cpp', 'add_executable(test_ctor_slot_365 tests/test_ctor_slot_365.cpp)', 'add_test(NAME ctor_slot_365 COMMAND test_ctor_slot_365)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Single bool console variable `nico_freezeBoss` (object 0x32524c8, value 0/0, functor tag 0x15cd220).'))
