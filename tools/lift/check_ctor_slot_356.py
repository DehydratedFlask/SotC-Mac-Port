#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 356 (RVA 0x78e2f0, 299 bytes), lane L4.

One bool console variable `shadow_weapons_newWeaponCyclingLogic` (value 1, default 1, functor tag 0x15cdfb0, flags 1): 0x20af0, f590(desc), 0x20d60, f590(name), atexit(0x20b0).

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
    "slot": 356,
    "base": 0x78e2f0,
    "size": 299,
    "body_sha256": "6c150e4622bc0e78deddc89a635e3250975783ed69386758ffe2ad705de42003",
    "pad": "9090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15cdfb0],
    "float_tags": [],
    "strings": [
        (0x1284240, b'shadow_weapons_newWeaponCyclingLogic'),
        (0x128430a, b''),
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x78e301, 7, 3, 0x15fe518),  # stack canary cell
        ("rip", 0x78e308, 7, 3, 0x1594208),  # class vtable
        ("rip", 0x78e30f, 7, 3, 0x3256390),  # bss object/cell
        ("rip", 0x78e316, 7, 3, 0x1284240),  # rodata string
        ("rip", 0x78e31d, 7, 3, 0x128430a),  # rodata string
        ("imm", 0x78e330, "b901000000"),
        ("imm", 0x78e352, "48c7459824000000"),
        ("imm", 0x78e367, "48c745c800000000"),
        ("call", 0x78e36f, 0x20af0),
        ("rip", 0x78e374, 7, 3, 0x15940a0),  # class vtable
        ("rip", 0x78e389, 7, 3, 0x15cdfb0),  #
        ("imm", 0x78e39b, "c7436900000000"),
        ("imm", 0x78e3a7, "48c783a000000000000000"),
        ("imm", 0x78e3b9, "48c783b800000000000000"),
        ("call", 0x78e3c4, 0xf590),
        ("imm", 0x78e3cf, "c683c000000001"),
        ("imm", 0x78e3d6, "c6436801"),
        ("call", 0x78e3da, 0x20d60),
        ("call", 0x78e3e2, 0xf590),
        ("rip", 0x78e3e7, 7, 3, 0x20b0),  # destructor/function RVA
        ("rip", 0x78e3ee, 7, 3, 0x1604218),  # dso handle
        ("call", 0x78e3f8, 0xf8f390),
        ("call", 0x78e416, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_356_78e2f0.hpp', 'source': 'native/core/ctor_slot_356_78e2f0.cpp', 'test': 'native/tests/test_ctor_slot_356.cpp', 'shapes_h': 'native/core/ctor_shapes_343_356.hpp', 'shapes_c': 'native/core/ctor_shapes_343_356.cpp', 'cmake': 'native/CMakeLists.txt', 'consts': {'kSlot356Rva': 7922416, 'kSlot356Size': 299}, 'header_tokens': ['kSlot356FnTag = 0x15cdfb0'], 'source_order': ['init_bool_var_site(objects.var, kSlot356Name, kSlot356FnTag, concmd, 1, 1)'], 'test_tokens': ['kSlot356Rva == 0x78e2f0', 'section_alone()', 'section_failures()'], 'shapes_tokens': [], 'cmake_tokens': ['core/ctor_slot_356_78e2f0.cpp', 'add_executable(test_ctor_slot_356 tests/test_ctor_slot_356.cpp)', 'add_test(NAME ctor_slot_356 COMMAND test_ctor_slot_356)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'One bool console variable `shadow_weapons_newWeaponCyclingLogic` (value 1, default 1, functor tag 0x15cdfb0, flags 1): 0x20af0, f590(desc), 0x20d60, f590(name), atexit(0x20b0).'))
