#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 439 (RVA 0x4f7b70, 410 bytes), lane m3.

Bool console variable `r_waterWetHelperDebugDraw` (object 0x2efc180, value 0, default 0, functor tag 0x15c4a50) and a debug-menu toggle built by 0x26db0
(item 0x2efc248) under the parent path `Water`.

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
    "slot": 439,
    "base": 0x4f7b70,
    "size": 410,
    "body_sha256": "d4d4df794ae64569257f307de455fa4821e42de4c59aa28f81ac4fff36e33e8e",
    "pad": "909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_from_var_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15c4a50],
    "float_tags": [],
    "strings": [
        (0x100c970, b'r_waterWetHelperDebugDraw'),
        (0x100c9b0, b''),
        (0x100c98a, b'Water'),
    ],
    "lengths": [
        ('r_waterWetHelperDebugDraw', 0x4f7bd2, "48c7459819000000"),
        ('Water', 0x4f7cc0, "48c745b805000000"),
    ],
    "pins": [
        ("rip", 0x4f7b81, 7, 3, 0x15fe518),
        ("rip", 0x4f7b88, 7, 3, 0x1594208),
        ("rip", 0x4f7b8f, 7, 3, 0x2efc180),
        ("rip", 0x4f7b96, 7, 3, 0x100c970),
        ("rip", 0x4f7b9d, 7, 3, 0x100c9b0),
        ("imm", 0x4f7bb0, "b901000000"),
        ("imm", 0x4f7bd2, "48c7459819000000"),
        ("rip", 0x4f7be4, 7, 3, 0x2efc180),
        ("imm", 0x4f7beb, "48c745c800000000"),
        ("call", 0x4f7bf3, 0x20af0),
        ("rip", 0x4f7bf8, 7, 3, 0x15940a0),
        ("rip", 0x4f7c02, 7, 3, 0x2efc240),
        ("rip", 0x4f7c0d, 7, 3, 0x15c4a50),
        ("rip", 0x4f7c18, 7, 3, 0x2efc180),
        ("rip", 0x4f7c1f, 7, 3, 0x2efc1e0),
        ("rip", 0x4f7c26, 10, 2, 0x2efc1e9),
        ("imm", 0x4f7c26, "c705b945a00200000000"),
        ("rip", 0x4f7c30, 8, 4, 0x2efc1f0),
        ("imm", 0x4f7c30, "c5fc1105b845a002"),
        ("rip", 0x4f7c38, 11, 3, 0x2efc220),
        ("imm", 0x4f7c38, "48c705dd45a00200000000"),
        ("rip", 0x4f7c43, 7, 3, 0x2efc230),
        ("rip", 0x4f7c4a, 11, 3, 0x2efc238),
        ("imm", 0x4f7c4a, "48c705e345a00200000000"),
        ("call", 0x4f7c55, 0xf590),
        ("rip", 0x4f7c5d, 7, 3, 0x2efc180),
        ("rip", 0x4f7c64, 7, 2, 0x2efc240),
        ("imm", 0x4f7c64, "c605d545a00200"),
        ("rip", 0x4f7c6b, 7, 2, 0x2efc1e8),
        ("imm", 0x4f7c6b, "c6057645a00200"),
        ("call", 0x4f7c72, 0x20d60),
        ("call", 0x4f7c7a, 0xf590),
        ("rip", 0x4f7c7f, 7, 3, 0x1604218),
        ("rip", 0x4f7c86, 7, 3, 0x20b0),
        ("call", 0x4f7c93, 0xf8f390),
        ("rip", 0x4f7c98, 7, 3, 0x2efc248),
        ("rip", 0x4f7ca3, 7, 3, 0x100c98a),
        ("imm", 0x4f7cae, "b930000000"),
        ("imm", 0x4f7cc0, "48c745b805000000"),
        ("call", 0x4f7ccd, 0x26db0),
        ("call", 0x4f7cd5, 0xf590),
        ("rip", 0x4f7cda, 7, 3, 0x272d0),
        ("call", 0x4f7ce7, 0xf8f390),
        ("call", 0x4f7d05, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_439_4f7b70.hpp', 'source': 'native/core/ctor_slot_439_4f7b70.cpp', 'test': 'native/tests/test_ctor_slot_439.cpp', 'cmake': 'native/lanes/m3.cmake', 'consts': {'kSlot439Rva': 5208944, 'kSlot439Size': 410}, 'header_tokens': ['kSlot439FnTag = 0x15c4a50', 'kSlot439ObjectRva = 0x2efc180', 'kSlot439ItemRva = 0x2efc248'], 'source_order': ['init_bool_var_site(objects.var, kSlot439VarName, kSlot439FnTag, concmd, 0, 0)', 'init_toggle_site_26db0(objects.item, objects.var, kSlot439Parent, concmd, menu)'], 'test_tokens': ['kSlot439Rva == 0x4f7b70', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_439_4f7b70.cpp', 'add_executable(test_ctor_slot_439 tests/test_ctor_slot_439.cpp)', 'add_test(NAME ctor_slot_439 COMMAND test_ctor_slot_439)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'bool variable `r_waterWetHelperDebugDraw` (value 0, tag 0x15c4a50) + 0x26db0 toggle under `Water` (flags 0x30, f590(parent), atexit 0x272d0).'))
