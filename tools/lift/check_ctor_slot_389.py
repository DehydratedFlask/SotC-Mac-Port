#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 389 (RVA 0x695a50, 386 bytes), lane M2.

Bool variable `shadow_mungeFishFinCloth` (functor tag 0x15cc690, value/default 1, flags 1) and its 0x26db0 menu toggle under `Shadow Cloth` (flags 0x30).

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
    "slot": 389,
    "base": 0x695a50,
    "size": 386,
    "body_sha256": "db45e912eb4c774106b27ae73be190f09c69a0d84244b32f81c1cba18d73df7b",
    "pad": "9090909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_from_var_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15cc690],
    "float_tags": [],
    "strings": [
        (0x12723a0, b'shadow_mungeFishFinCloth'),
        (0x12724f2, b''),
        (0x12723b9, b'Shadow Cloth'),
    ],
    "lengths": [
        ('shadow_mungeFishFinCloth', 0x695ab2, "48c7459818000000"),
        ('Shadow Cloth', 0x695b85, "48c745b80c000000"),
    ],
    "pins": [
        ("rip", 0x695a61, 7, 3, 0x15fe518),
        ("rip", 0x695a68, 7, 3, 0x1594208),
        ("rip", 0x695a6f, 7, 3, 0x3015ac8),
        ("rip", 0x695a76, 7, 3, 0x12723a0),
        ("rip", 0x695a7d, 7, 3, 0x12724f2),
        ("imm", 0x695a90, "b901000000"),
        ("imm", 0x695a99, "4983c410"),
        ("imm", 0x695ab2, "48c7459818000000"),
        ("imm", 0x695ac4, "4c8923"),
        ("imm", 0x695ac7, "48c745c800000000"),
        ("call", 0x695acf, 0x20af0),
        ("rip", 0x695ad4, 7, 3, 0x15940a0),
        ("imm", 0x695ade, "488d93c0000000"),
        ("rip", 0x695ae9, 7, 3, 0x15cc690),
        ("imm", 0x695af0, "4883c010"),
        ("imm", 0x695af4, "488903"),
        ("imm", 0x695af7, "48895360"),
        ("imm", 0x695afb, "c7436900000000"),
        ("imm", 0x695b02, "c5fc114370"),
        ("imm", 0x695b07, "48c783a000000000000000"),
        ("imm", 0x695b12, "48898bb0000000"),
        ("imm", 0x695b19, "48c783b800000000000000"),
        ("call", 0x695b24, 0xf590),
        ("imm", 0x695b2c, "4c8923"),
        ("imm", 0x695b2f, "c683c000000001"),
        ("imm", 0x695b36, "c6436801"),
        ("call", 0x695b3a, 0x20d60),
        ("call", 0x695b42, 0xf590),
        ("rip", 0x695b47, 7, 3, 0x1604218),
        ("rip", 0x695b4e, 7, 3, 0x20b0),
        ("call", 0x695b5b, 0xf8f390),
        ("rip", 0x695b60, 7, 3, 0x3015b90),
        ("rip", 0x695b6b, 7, 3, 0x12723b9),
        ("imm", 0x695b76, "b930000000"),
        ("imm", 0x695b85, "48c745b80c000000"),
        ("call", 0x695b95, 0x26db0),
        ("call", 0x695b9d, 0xf590),
        ("rip", 0x695ba2, 7, 3, 0x272d0),
        ("call", 0x695baf, 0xf8f390),
        ("call", 0x695bcd, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_389_695a50.hpp',
     'source': 'native/core/ctor_slot_389_695a50.cpp',
     'test': 'native/tests/test_ctor_slot_389.cpp',
     'cmake': 'native/lanes/m2.cmake',
     'consts': {'kSlot389Rva': 6904400, 'kSlot389Size': 386},
     'header_tokens': ['kSlot389FnTag = 0x15cc690'],
     'source_order': ['init_bool_var_site(objects.var, kSlot389VarName, kSlot389FnTag, concmd, 1, 1)',
                      'init_toggle_site_26db0(objects.item, objects.var, kSlot389Parent, concmd, menu)'],
     'test_tokens': ['kSlot389Rva == 0x695a50', 'section_alone()', 'section_failures()'],
     'cmake_tokens': ['core/ctor_slot_389_695a50.cpp',
                      'add_executable(test_ctor_slot_389 tests/test_ctor_slot_389.cpp)',
                      'add_test(NAME ctor_slot_389 COMMAND test_ctor_slot_389)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bool variable `shadow_mungeFishFinCloth` + 0x26db0 `Shadow Cloth` toggle (tag 0x15cc690).'))
