#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 403 (RVA 0x5f0160, 386 bytes), lane M2.

Bool variable `shadow_drawModelCharacterClasses` (functor tag 0x15cc078, value/default 0, flags 1) and its 0x26db0 menu toggle under `Shadow Misc` (flags 0x30).

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
    "slot": 403,
    "base": 0x5f0160,
    "size": 386,
    "body_sha256": "21962eb4a47fdf7de0364a8b473c978a1ce3811b53cd10b8054a8de18eb49e77",
    "pad": "9090909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_from_var_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15cc078],
    "float_tags": [],
    "strings": [
        (0x126a2c4, b'shadow_drawModelCharacterClasses'),
        (0x126a4e8, b''),
        (0x126a2e5, b'Shadow Misc'),
    ],
    "lengths": [
        ('shadow_drawModelCharacterClasses', 0x5f01c2, "48c7459820000000"),
        ('Shadow Misc', 0x5f0295, "48c745b80b000000"),
    ],
    "pins": [
        ("rip", 0x5f0171, 7, 3, 0x15fe518),
        ("rip", 0x5f0178, 7, 3, 0x1594208),
        ("rip", 0x5f017f, 7, 3, 0x300c168),
        ("rip", 0x5f0186, 7, 3, 0x126a2c4),
        ("rip", 0x5f018d, 7, 3, 0x126a4e8),
        ("imm", 0x5f01a0, "b901000000"),
        ("imm", 0x5f01a9, "4983c410"),
        ("imm", 0x5f01c2, "48c7459820000000"),
        ("imm", 0x5f01d4, "4c8923"),
        ("imm", 0x5f01d7, "48c745c800000000"),
        ("call", 0x5f01df, 0x20af0),
        ("rip", 0x5f01e4, 7, 3, 0x15940a0),
        ("imm", 0x5f01ee, "488d93c0000000"),
        ("rip", 0x5f01f9, 7, 3, 0x15cc078),
        ("imm", 0x5f0200, "4883c010"),
        ("imm", 0x5f0204, "488903"),
        ("imm", 0x5f0207, "48895360"),
        ("imm", 0x5f020b, "c7436900000000"),
        ("imm", 0x5f0212, "c5fc114370"),
        ("imm", 0x5f0217, "48c783a000000000000000"),
        ("imm", 0x5f0222, "48898bb0000000"),
        ("imm", 0x5f0229, "48c783b800000000000000"),
        ("call", 0x5f0234, 0xf590),
        ("imm", 0x5f023c, "4c8923"),
        ("imm", 0x5f023f, "c683c000000000"),
        ("imm", 0x5f0246, "c6436800"),
        ("call", 0x5f024a, 0x20d60),
        ("call", 0x5f0252, 0xf590),
        ("rip", 0x5f0257, 7, 3, 0x1604218),
        ("rip", 0x5f025e, 7, 3, 0x20b0),
        ("call", 0x5f026b, 0xf8f390),
        ("rip", 0x5f0270, 7, 3, 0x300c230),
        ("rip", 0x5f027b, 7, 3, 0x126a2e5),
        ("imm", 0x5f0286, "b930000000"),
        ("imm", 0x5f0295, "48c745b80b000000"),
        ("call", 0x5f02a5, 0x26db0),
        ("call", 0x5f02ad, 0xf590),
        ("rip", 0x5f02b2, 7, 3, 0x272d0),
        ("call", 0x5f02bf, 0xf8f390),
        ("call", 0x5f02dd, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_403_5f0160.hpp',
     'source': 'native/core/ctor_slot_403_5f0160.cpp',
     'test': 'native/tests/test_ctor_slot_403.cpp',
     'cmake': 'native/lanes/m2.cmake',
     'consts': {'kSlot403Rva': 6226272, 'kSlot403Size': 386},
     'header_tokens': ['kSlot403FnTag = 0x15cc078'],
     'source_order': ['init_bool_var_site(objects.var, kSlot403VarName, kSlot403FnTag, concmd, 0, 0)',
                      'init_toggle_site_26db0(objects.item, objects.var, kSlot403Parent, concmd, menu)'],
     'test_tokens': ['kSlot403Rva == 0x5f0160', 'section_alone()', 'section_failures()'],
     'cmake_tokens': ['core/ctor_slot_403_5f0160.cpp',
                      'add_executable(test_ctor_slot_403 tests/test_ctor_slot_403.cpp)',
                      'add_test(NAME ctor_slot_403 COMMAND test_ctor_slot_403)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bool variable `shadow_drawModelCharacterClasses` + 0x26db0 `Shadow Misc` toggle (tag 0x15cc078).'))
