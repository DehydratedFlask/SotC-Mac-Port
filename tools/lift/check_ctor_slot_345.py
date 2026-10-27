#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 345 (RVA 0x7cec60, 431 bytes), lane L4.

Bool variable `shadow_debugPlayerControllerState` (value 0 / default 0, tag 0x15ce390) plus a 0x26ea0 toggle (flags 0x30), text released before label, atexit 0x272d0.

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
    "slot": 345,
    "base": 0x7cec60,
    "size": 431,
    "body_sha256": "859697f423cc6e83043637ce0cfd5458a3cc7681c82a4f2f759d6e7935c39bd3",
    "pad": "90",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15ce390],
    "float_tags": [],
    "strings": [
        (0x128872c, b'shadow_debugPlayerControllerState'),
        (0x12887f9, b''),
        (0x128875c, b'Debug Player Controller State'),
        (0x128874e, b'Shadow Player'),
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x7cec71, 7, 3, 0x15fe518),  # stack canary cell
        ("rip", 0x7cec78, 7, 3, 0x1594208),  # class vtable
        ("rip", 0x7cec7f, 7, 3, 0x325e4b8),  # bss object/cell
        ("rip", 0x7cec86, 7, 3, 0x128872c),  # rodata string
        ("rip", 0x7cec8d, 7, 3, 0x12887f9),  # rodata string
        ("imm", 0x7ceca0, "b901000000"),
        ("imm", 0x7cecc1, "48c7459821000000"),
        ("imm", 0x7cecd6, "48c745c800000000"),
        ("call", 0x7cecde, 0x20af0),
        ("rip", 0x7cece3, 7, 3, 0x15940a0),  # class vtable
        ("rip", 0x7cecf8, 7, 3, 0x15ce390),  # functor vtable tag
        ("imm", 0x7ced0a, "c7436900000000"),
        ("imm", 0x7ced16, "48c783a000000000000000"),
        ("imm", 0x7ced28, "48c783b800000000000000"),
        ("call", 0x7ced33, 0xf590),
        ("imm", 0x7ced3e, "c683c000000000"),
        ("imm", 0x7ced45, "c6436800"),
        ("call", 0x7ced49, 0x20d60),
        ("call", 0x7ced51, 0xf590),
        ("rip", 0x7ced56, 7, 3, 0x1604218),  # dso handle
        ("rip", 0x7ced5d, 7, 3, 0x20b0),  # destructor/function RVA
        ("call", 0x7ced6a, 0xf8f390),
        ("rip", 0x7ced6f, 7, 3, 0x128875c),  # rodata string
        ("rip", 0x7ced76, 7, 3, 0x325e580),  # bss object/cell
        ("rip", 0x7ced85, 7, 3, 0x128874e),  # rodata string
        ("imm", 0x7ced90, "41b830000000"),
        ("imm", 0x7cedaa, "48c745b80d000000"),
        ("imm", 0x7cedb2, "48c745981d000000"),
        ("call", 0x7cedc4, 0x26ea0),
        ("call", 0x7cedcc, 0xf590),
        ("call", 0x7cedd4, 0xf590),
        ("rip", 0x7cedd9, 7, 3, 0x272d0),  # destructor/function RVA
        ("call", 0x7cede6, 0xf8f390),
        ("rip", 0x7cedeb, 7, 3, 0x15fe518),  # stack canary cell
        ("call", 0x7cee0a, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_345_7cec60.hpp', 'source': 'native/core/ctor_slot_345_7cec60.cpp', 'test': 'native/tests/test_ctor_slot_345.cpp', 'shapes_h': 'native/core/ctor_shapes_343_356.hpp', 'shapes_c': 'native/core/ctor_shapes_343_356.cpp', 'cmake': 'native/CMakeLists.txt', 'consts': {'kSlot345Rva': 8186976, 'kSlot345Size': 431}, 'header_tokens': ['kSlot345FnTag = 0x15ce390'], 'source_order': ['init_bool_var_site(objects.var, kSlot345Name, kSlot345FnTag, concmd, 0, 0)', 'init_toggle_site_26ea0(objects.item, objects.var, kSlot345Label, kSlot345Text, concmd, menu)'], 'test_tokens': ['kSlot345Rva == 0x7cec60', 'section_alone()', 'section_failures()'], 'shapes_tokens': ['release_inert_view_f590(t) && release_inert_view_f590(l)', 'init_debug_menu_item_26ea0(item, l, t, debug_menu_target_for(target), kL4MenuItemFlags)'], 'cmake_tokens': ['core/ctor_slot_345_7cec60.cpp', 'add_executable(test_ctor_slot_345 tests/test_ctor_slot_345.cpp)', 'add_test(NAME ctor_slot_345 COMMAND test_ctor_slot_345)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bool variable `shadow_debugPlayerControllerState` (value 0 / default 0, tag 0x15ce390) plus a 0x26ea0 toggle (flags 0x30), text released before label, atexit 0x272d0.'))
