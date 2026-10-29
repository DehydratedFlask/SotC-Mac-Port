#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 364 (RVA 0x74a8e0, 497 bytes), lane M1.

Unbounded float var `cp10_WanderSheathCollisionSwordScalar` (0x6aa00, functor 0x15cd240) + 0x27050 slider item `CP10-Anim` / `WanderSheathCollisionSwordScalar` range 0.01..0.1.

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

lib.COMMON_CALLEES["debug_item_float_ctor"] = (0x27050, 0xD0, "ad7ca05e3df035fa5df336f0463fb63e331566f140d6f0bf3ad16e9e2766b053")
SPEC = {
    "slot": 364,
    "base": 0x74a8e0,
    "size": 497,
    "body_sha256": "84fba2f4fd664434efa15eeb15f64fe0be9cf51dfaa314033b218de8505a9c20",
    "pad": "909090909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'float_dtor', 'release_f590', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy', 'unbounded_float_ctor', 'menu_item_dtor', 'debug_item_float_ctor'],
    "bool_tags": [],
    "float_tags": [22860352],
    "strings": [
        (0x127fa50, b'cp10_WanderSheathCollisionSwordScalar'),
        (0x127faee, b''),
        (0x127fa76, b'CP10-Anim'),
        (0x127fa80, b'WanderSheathCollisionSwordScalar'),
    ],
    "lengths": [
        ('cp10_WanderSheathCollisionSwordScalar', 0x74a954, "48c744240825000000"),
        ('CP10-Anim', 0x74aa50, "48c744244809000000"),
        ('WanderSheathCollisionSwordScalar', 0x74aa59, "48c744242820000000"),
    ],
    "pins": [
        ("rip", 0x74a8f8, 7, 3, 0x15fe518),
        ("rip", 0x74a8ff, 7, 3, 0x15942a0),
        ("rip", 0x74a906, 7, 3, 0x127fa50),
        ("rip", 0x74a911, 7, 3, 0x32525b0),
        ("rip", 0x74a91d, 7, 3, 0x15cd240),
        ("imm", 0x74a931, "b901000000"),
        ("imm", 0x74a954, "48c744240825000000"),
        ("rip", 0x74a963, 7, 3, 0x127faee),
        ("imm", 0x74a96e, "48c744247000000000"),
        ("rip", 0x74a977, 7, 3, 0x32525b0),
        ("rip", 0x74a991, 7, 3, 0x3252680),
        ("imm", 0x74a9a1, "48c784248800000000000000"),
        ("imm", 0x74a9ad, "48c744243800000000"),
        ("call", 0x74a9b6, 0x6aa00),
        ("call", 0x74a9be, 0xf590),
        ("imm", 0x74a9ce, "ff5018"),
        ("rip", 0x74a9d4, 7, 3, 0x32525b0),
        ("rip", 0x74a9db, 10, 2, 0x3252680),
        ("rip", 0x74a9e5, 10, 2, 0x3252618),
        ("call", 0x74a9ef, 0x20d60),
        ("call", 0x74a9f7, 0xf590),
        ("rip", 0x74a9fc, 7, 3, 0x1604218),
        ("rip", 0x74aa03, 7, 3, 0x20f0),
        ("call", 0x74aa10, 0xf8f390),
        ("rip", 0x74aa15, 7, 3, 0x127fa76),
        ("rip", 0x74aa1c, 7, 3, 0x127fa80),
        ("rip", 0x74aa27, 8, 4, 0x127f9f0),
        ("rip", 0x74aa2f, 7, 3, 0x3252688),
        ("imm", 0x74aa40, "41b830000000"),
        ("imm", 0x74aa50, "48c744244809000000"),
        ("imm", 0x74aa59, "48c744242820000000"),
        ("rip", 0x74aa6e, 8, 4, 0x127f9ec),
        ("call", 0x74aa82, 0x27050),
        ("call", 0x74aa8a, 0xf590),
        ("call", 0x74aa92, 0xf590),
        ("rip", 0x74aa97, 7, 3, 0x272d0),
        ("call", 0x74aaa4, 0xf8f390),
        ("rip", 0x74aaa9, 7, 3, 0x15fe518),
        ("call", 0x74aacc, 0xf8f3a0),
        ("imm", 0x127f9f0, "cdcccc3d"),
        ("imm", 0x127f9ec, "0ad7233c"),
    ],
    "native": {'header': 'native/core/ctor_slot_364_74a8e0.hpp', 'source': 'native/core/ctor_slot_364_74a8e0.cpp', 'test': 'native/tests/test_ctor_slot_364.cpp', 'shapes_h': 'native/core/ctor_shapes_343_356.hpp', 'shapes_c': 'native/core/lane_m1_float_item.cpp', 'cmake': 'native/lanes/m1.cmake', 'consts': {'kSlot364Rva': 7645408, 'kSlot364Size': 497}, 'header_tokens': ['kSlot364ItemFlags = 0x30', 'kSlot364RangeMinBits = 0x3c23d70au', 'kSlot364RangeMaxBits = 0x3dcccccdu', 'kFloatArgFnVtblSlot364', 'kSlot364ObjectRva = 0x32525b0', 'kSlot364ItemRva = 0x3252688'], 'source_order': ['init_float_var_static(objects.var, kSlot364Spec, concmd)', 'lane_m1::init_item_27050_site(objects.item, &objects.var, kSlot364ItemLabel, kSlot364ItemText, kSlot364ItemFlags, lo,'], 'test_tokens': ['kSlot364Rva == 0x74a8e0', 'section_alone()', 'section_failures()', 'section_functor_whitelist()'], 'shapes_tokens': ['kFloatArgFnVtblSlot364 = 0x15cd240', 'init_debug_menu_item_27050(item, l, t, target, flags, range_min, range_max)', 'register_debug_menu_item_268b0(item, menu, registry.atexit_log)', 'release_inert_view_f590(t) && release_inert_view_f590(l)', 'registry.atexit_log.record(kDebugMenuItemDtorRva, &item)'], 'cmake_tokens': ['core/ctor_slot_364_74a8e0.cpp', 'add_executable(test_ctor_slot_364 tests/test_ctor_slot_364.cpp)', 'add_test(NAME ctor_slot_364 COMMAND test_ctor_slot_364)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Unbounded float var `cp10_WanderSheathCollisionSwordScalar` (0x6aa00, functor 0x15cd240) + 0x27050 slider item `CP10-Anim` / `WanderSheathCollisionSwordScalar` range 0.01..0.1.'))
