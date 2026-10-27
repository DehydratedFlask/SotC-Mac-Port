#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 353 (RVA 0x7a14d0, 569 bytes), lane L4.

Bool variable `shadow_camera_normal_overrideDefaultPitchOffset` (tag 0x15ce1a0, 1/1) then unbounded float `shadow_camera_normal_defaultPitchOffset` (0x6aa00, functor 0x15ce1c0, default 0).

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
    "slot": 353,
    "base": 0x7a14d0,
    "size": 569,
    "body_sha256": "72cf56dae35de014b7d8f97efde39d091a708a7241e6e726e8dbcaea3e0a4001",
    "pad": "90909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'float_dtor', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy', 'unbounded_float_ctor'],
    "bool_tags": [0x15ce1a0],
    "float_tags": [0x15ce1c0],
    "strings": [
        (0x12863a0, b'shadow_camera_normal_overrideDefaultPitchOffset'),
        (0x128640f, b''),
        (0x12863d0, b'shadow_camera_normal_defaultPitchOffset'),
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x7a14e8, 7, 3, 0x15fe518),  # stack canary cell
        ("rip", 0x7a14ef, 7, 3, 0x1594208),  # class vtable
        ("rip", 0x7a14f6, 7, 3, 0x325a428),  # bss object/cell
        ("rip", 0x7a1502, 7, 3, 0x12863a0),  # rodata string
        ("rip", 0x7a1512, 7, 3, 0x128640f),  # rodata string
        ("imm", 0x7a1519, "b901000000"),
        ("imm", 0x7a153b, "48c74424282f000000"),
        ("imm", 0x7a1558, "48c744245800000000"),
        ("call", 0x7a1561, 0x20af0),
        ("rip", 0x7a1566, 7, 3, 0x15940a0),  # class vtable
        ("rip", 0x7a157b, 7, 3, 0x15ce1a0),  # functor vtable tag
        ("imm", 0x7a158d, "c7436900000000"),
        ("imm", 0x7a1599, "48c783a000000000000000"),
        ("imm", 0x7a15ab, "48c783b800000000000000"),
        ("call", 0x7a15b6, 0xf590),
        ("imm", 0x7a15c1, "c683c000000001"),
        ("imm", 0x7a15c8, "c6436801"),
        ("call", 0x7a15cc, 0x20d60),
        ("call", 0x7a15d4, 0xf590),
        ("rip", 0x7a15d9, 7, 3, 0x20b0),  # destructor/function RVA
        ("rip", 0x7a15e0, 7, 3, 0x1604218),  # dso handle
        ("call", 0x7a15ea, 0xf8f390),
        ("rip", 0x7a15ef, 7, 3, 0x15942a0),  # class vtable
        ("rip", 0x7a15f6, 7, 3, 0x325a4f0),  # bss object/cell
        ("rip", 0x7a15fd, 7, 3, 0x12863d0),  # rodata string
        ("imm", 0x7a161f, "b901000000"),
        ("imm", 0x7a1624, "48c744247000000000"),
        ("rip", 0x7a1636, 7, 3, 0x15ce1c0),  # functor vtable tag
        ("imm", 0x7a1643, "48c744240827000000"),
        ("imm", 0x7a165e, "48c744243800000000"),
        ("imm", 0x7a166f, "48c784248800000000000000"),
        ("call", 0x7a168c, 0x6aa00),
        ("call", 0x7a1694, 0xf590),
        ("imm", 0x7a16a4, "ff5018"),
        ("imm", 0x7a16ad, "c783d000000000000000"),
        ("imm", 0x7a16b7, "c7436800000000"),
        ("call", 0x7a16be, 0x20d60),
        ("call", 0x7a16c6, 0xf590),
        ("rip", 0x7a16cb, 7, 3, 0x20f0),  # destructor/function RVA
        ("rip", 0x7a16d2, 7, 3, 0x1604218),  # dso handle
        ("call", 0x7a16dc, 0xf8f390),
        ("rip", 0x7a16e1, 7, 3, 0x15fe518),  # stack canary cell
        ("call", 0x7a1704, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_353_7a14d0.hpp', 'source': 'native/core/ctor_slot_353_7a14d0.cpp', 'test': 'native/tests/test_ctor_slot_353.cpp', 'shapes_h': 'native/core/ctor_shapes_343_356.hpp', 'shapes_c': 'native/core/ctor_shapes_343_356.cpp', 'cmake': 'native/CMakeLists.txt', 'consts': {'kSlot353Rva': 8000720, 'kSlot353Size': 569}, 'header_tokens': ['kSlot353BoolFnTag = 0x15ce1a0', 'kFloatArgFnVtbl_353B', 'false,'], 'source_order': ['init_bool_var_site(objects.flag, kSlot353BoolName, kSlot353BoolFnTag, concmd, 1, 1)', 'init_float_var_static(objects.offset, kSlot353FloatSpec, concmd)'], 'test_tokens': ['kSlot353Rva == 0x7a14d0', 'section_alone()', 'section_failures()'], 'shapes_tokens': ['init_unbounded_float_var_6aa00(var, name, &var.value, spec.flags, args, description)'], 'cmake_tokens': ['core/ctor_slot_353_7a14d0.cpp', 'add_executable(test_ctor_slot_353 tests/test_ctor_slot_353.cpp)', 'add_test(NAME ctor_slot_353 COMMAND test_ctor_slot_353)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bool variable `shadow_camera_normal_overrideDefaultPitchOffset` (tag 0x15ce1a0, 1/1) then unbounded float `shadow_camera_normal_defaultPitchOffset` (0x6aa00, functor 0x15ce1c0, default 0).'))
