#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 413 (RVA 0x57c810, 370 bytes), lane M2.

Bounded float variable `shadow_deadZoneDefault` through 0xc300 (min 0, max 1.0, default 0x3e780347, functor 0x15cb148, flags 1).

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
    "slot": 413,
    "base": 0x57c810,
    "size": 370,
    "body_sha256": "7a2e39115c0d193a5e47bf6067a0edd0e2bfb84286e24fd18505d6c96bc4ba14",
    "pad": "9090909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'float_dtor', 'release_f590', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy', 'bounded_float_ctor'],
    "bool_tags": [],
    "float_tags": [0x15cb148],
    "strings": [
        (0x10244b0, b'shadow_deadZoneDefault'),
        (0x10244cc, b''),
    ],
    "lengths": [
        ('shadow_deadZoneDefault', 0x57c891, "48c744242816000000"),
    ],
    "pins": [
        ("rip", 0x57c828, 7, 3, 0x15fe518),
        ("rip", 0x57c82f, 7, 3, 0x15942a0),
        ("rip", 0x57c836, 7, 3, 0x2f358c8),
        ("rip", 0x57c83d, 7, 3, 0x10244b0),
        ("rip", 0x57c851, 7, 3, 0x15cb148),
        ("imm", 0x57c86a, "41b901000000"),
        ("imm", 0x57c876, "4983c510"),
        ("imm", 0x57c87a, "4889df"),
        ("rip", 0x57c88a, 7, 3, 0x10244cc),
        ("imm", 0x57c891, "48c744242816000000"),
        ("imm", 0x57c89a, "48c784249000000000000000"),
        ("imm", 0x57c8ba, "c744241c00000000"),
        ("imm", 0x57c8c2, "c74424180000803f"),
        ("imm", 0x57c8ca, "4c892b"),
        ("imm", 0x57c8cd, "48c78424a800000000000000"),
        ("imm", 0x57c8f0, "48c744245800000000"),
        ("call", 0x57c901, 0xc300),
        ("call", 0x57c90d, 0xf590),
        ("imm", 0x57c91d, "ff5018"),
        ("imm", 0x57c920, "4889df"),
        ("imm", 0x57c923, "4c892b"),
        ("imm", 0x57c926, "c783d00000004703783e"),
        ("imm", 0x57c930, "c743684703783e"),
        ("call", 0x57c937, 0x20d60),
        ("call", 0x57c93f, 0xf590),
        ("rip", 0x57c944, 7, 3, 0x20f0),
        ("rip", 0x57c94b, 7, 3, 0x1604218),
        ("call", 0x57c955, 0xf8f390),
        ("rip", 0x57c95a, 7, 3, 0x15fe518),
        ("call", 0x57c97d, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_413_57c810.hpp',
     'source': 'native/core/ctor_slot_413_57c810.cpp',
     'test': 'native/tests/test_ctor_slot_413.cpp',
     'cmake': 'native/lanes/m2.cmake',
     'shapes_h': 'native/core/lane_m2_shapes.hpp',
     'shapes_c': 'native/core/lane_m2_shapes.cpp',
     'consts': {'kSlot413Rva': 5752848, 'kSlot413Size': 370},
     'header_tokens': ['kSlot413FnTag = 0x15cb148', '0x3f800000u, 0x3e780347u'],
     'source_order': ['lane_m2::init_float_var_site(objects.var, kSlot413Spec, concmd)'],
     'shapes_tokens': ['vtbl == kFloatArgFnVtbl_15cb148', 'shared.arg_fn_vtbl = kFloatArgFnVtbl_347;'],
     'test_tokens': ['kSlot413Rva == 0x57c810', 'section_alone()', 'section_failures()', 'section_unknown_functor_refused()'],
     'cmake_tokens': ['core/ctor_slot_413_57c810.cpp',
                      'add_executable(test_ctor_slot_413 tests/test_ctor_slot_413.cpp)',
                      'add_test(NAME ctor_slot_413 COMMAND test_ctor_slot_413)',
                      'core/lane_m2_shapes.cpp']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bounded float variable `shadow_deadZoneDefault` through 0xc300 (min 0, max 1.0, default 0x3e780347, functor 0x15cb148).'))
