#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 377 (RVA 0x6de050, 299 bytes), lane M1.

Single bool console variable `pauseGameIfControllerDisconnected` (object 0x3240cd0, value 1/1, functor tag 0x15cc998).

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
    "slot": 377,
    "base": 0x6de050,
    "size": 299,
    "body_sha256": "5d72d333308327a7359e95028e36fd18926f887bed0b2f7745f70c3301ab7fb6",
    "pad": "9090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [22858136],
    "float_tags": [],
    "strings": [
        (0x1276511, b'pauseGameIfControllerDisconnected'),
        (0x1276558, b''),
    ],
    "lengths": [
        ('pauseGameIfControllerDisconnected', 0x6de0b2, "48c7459821000000"),
    ],
    "pins": [
        ("rip", 0x6de061, 7, 3, 0x15fe518),
        ("rip", 0x6de068, 7, 3, 0x1594208),
        ("rip", 0x6de06f, 7, 3, 0x3240cd0),
        ("rip", 0x6de076, 7, 3, 0x1276511),
        ("rip", 0x6de07d, 7, 3, 0x1276558),
        ("imm", 0x6de090, "b901000000"),
        ("imm", 0x6de0b2, "48c7459821000000"),
        ("imm", 0x6de0c7, "48c745c800000000"),
        ("call", 0x6de0cf, 0x20af0),
        ("rip", 0x6de0d4, 7, 3, 0x15940a0),
        ("rip", 0x6de0e9, 7, 3, 0x15cc998),
        ("imm", 0x6de0fb, "c7436900000000"),
        ("imm", 0x6de107, "48c783a000000000000000"),
        ("imm", 0x6de119, "48c783b800000000000000"),
        ("call", 0x6de124, 0xf590),
        ("imm", 0x6de12f, "c683c000000001"),
        ("imm", 0x6de136, "c6436801"),
        ("call", 0x6de13a, 0x20d60),
        ("call", 0x6de142, 0xf590),
        ("rip", 0x6de147, 7, 3, 0x20b0),
        ("rip", 0x6de14e, 7, 3, 0x1604218),
        ("call", 0x6de158, 0xf8f390),
        ("call", 0x6de176, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_377_6de050.hpp', 'source': 'native/core/ctor_slot_377_6de050.cpp', 'test': 'native/tests/test_ctor_slot_377.cpp', 'cmake': 'native/lanes/m1.cmake', 'consts': {'kSlot377Rva': 7200848, 'kSlot377Size': 299}, 'header_tokens': ['kSlot377FnTag = 0x15cc998', 'kSlot377ObjectRva = 0x3240cd0'], 'source_order': ['init_bool_var_site(objects.var, kSlot377Name, kSlot377FnTag, concmd, 1, 1)'], 'test_tokens': ['kSlot377Rva == 0x6de050', 'section_alone()', 'section_failures()', 'section_with_neighbour()'], 'cmake_tokens': ['core/ctor_slot_377_6de050.cpp', 'add_executable(test_ctor_slot_377 tests/test_ctor_slot_377.cpp)', 'add_test(NAME ctor_slot_377 COMMAND test_ctor_slot_377)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Single bool console variable `pauseGameIfControllerDisconnected` (object 0x3240cd0, value 1/1, functor tag 0x15cc998).'))
