#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 383 (RVA 0x6c62b0, 386 bytes), lane M1.

Bool console variable `shadow_debugDrawCloth` (functor tag 0x15cc7d8, 0/0) + 0x26db0 menu toggle under `Shadow Cloth`.

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
    "slot": 383,
    "base": 0x6c62b0,
    "size": 386,
    "body_sha256": "f3b7128377fd5c80e52ebb27e9acfa03154d60d5fb73ae4324b2a35f520e63e9",
    "pad": "9090909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_from_var_ctor', 'menu_item_dtor'],
    "bool_tags": [22857688],
    "float_tags": [],
    "strings": [
        (0x1274fc0, b'shadow_debugDrawCloth'),
        (0x1274fee, b''),
        (0x1274fd6, b'Shadow Cloth'),
    ],
    "lengths": [
        ('shadow_debugDrawCloth', 0x6c6312, "48c7459815000000"),
        ('Shadow Cloth', 0x6c63e5, "48c745b80c000000"),
    ],
    "pins": [
        ("rip", 0x6c62c1, 7, 3, 0x15fe518),
        ("rip", 0x6c62c8, 7, 3, 0x1594208),
        ("rip", 0x6c62cf, 7, 3, 0x3239648),
        ("rip", 0x6c62d6, 7, 3, 0x1274fc0),
        ("rip", 0x6c62dd, 7, 3, 0x1274fee),
        ("imm", 0x6c62f0, "b901000000"),
        ("imm", 0x6c6312, "48c7459815000000"),
        ("imm", 0x6c6327, "48c745c800000000"),
        ("call", 0x6c632f, 0x20af0),
        ("rip", 0x6c6334, 7, 3, 0x15940a0),
        ("rip", 0x6c6349, 7, 3, 0x15cc7d8),
        ("imm", 0x6c635b, "c7436900000000"),
        ("imm", 0x6c6367, "48c783a000000000000000"),
        ("imm", 0x6c6379, "48c783b800000000000000"),
        ("call", 0x6c6384, 0xf590),
        ("imm", 0x6c638f, "c683c000000000"),
        ("imm", 0x6c6396, "c6436800"),
        ("call", 0x6c639a, 0x20d60),
        ("call", 0x6c63a2, 0xf590),
        ("rip", 0x6c63a7, 7, 3, 0x1604218),
        ("rip", 0x6c63ae, 7, 3, 0x20b0),
        ("call", 0x6c63bb, 0xf8f390),
        ("rip", 0x6c63c0, 7, 3, 0x3239710),
        ("rip", 0x6c63cb, 7, 3, 0x1274fd6),
        ("imm", 0x6c63d6, "b930000000"),
        ("imm", 0x6c63e5, "48c745b80c000000"),
        ("call", 0x6c63f5, 0x26db0),
        ("call", 0x6c63fd, 0xf590),
        ("rip", 0x6c6402, 7, 3, 0x272d0),
        ("call", 0x6c640f, 0xf8f390),
        ("call", 0x6c642d, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_383_6c62b0.hpp', 'source': 'native/core/ctor_slot_383_6c62b0.cpp', 'test': 'native/tests/test_ctor_slot_383.cpp', 'cmake': 'native/lanes/m1.cmake', 'consts': {'kSlot383Rva': 7103152, 'kSlot383Size': 386}, 'header_tokens': ['kSlot383FnTag = 0x15cc7d8', 'kSlot383ObjectRva = 0x3239648', 'kSlot383ItemRva = 0x3239710'], 'source_order': ['init_bool_var_site(objects.var, kSlot383VarName, kSlot383FnTag, concmd, 0, 0)', 'init_toggle_site_26db0(objects.item, objects.var, kSlot383Parent, concmd, menu)'], 'test_tokens': ['kSlot383Rva == 0x6c62b0', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_383_6c62b0.cpp', 'add_executable(test_ctor_slot_383 tests/test_ctor_slot_383.cpp)', 'add_test(NAME ctor_slot_383 COMMAND test_ctor_slot_383)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bool console variable `shadow_debugDrawCloth` (functor tag 0x15cc7d8, 0/0) + 0x26db0 menu toggle under `Shadow Cloth`.'))
