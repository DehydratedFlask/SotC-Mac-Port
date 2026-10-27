#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 350 (RVA 0x7af0b0, 610 bytes), lane L4.

Two float variables with functor 0x15ce260: bounded `nico_PhotoModeLwsZoomControlSpeed` (0xc300) and unbounded `shadow_bowZoomScaleOnHorseback` (0x6aa00).

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
    "slot": 350,
    "base": 0x7af0b0,
    "size": 610,
    "body_sha256": "eea65e87de90b6de5eda412c768e51bb5a7b808b2d5668b8b9191dfdcc59c77a",
    "pad": "9090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'float_dtor', 'release_f590', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy', 'bounded_float_ctor', 'unbounded_float_ctor'],
    "bool_tags": [],
    "float_tags": [0x15ce260],
    "strings": [
        (0x1286d60, b'nico_PhotoModeLwsZoomControlSpeed'),
        (0x1286db4, b''),
        (0x1286d82, b'shadow_bowZoomScaleOnHorseback'),
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x7af0c8, 7, 3, 0x15fe518),  # stack canary cell
        ("rip", 0x7af0cf, 7, 3, 0x15942a0),  # class vtable
        ("rip", 0x7af0d6, 7, 3, 0x325b5b0),  # bss object/cell
        ("rip", 0x7af0dd, 7, 3, 0x1286d60),  # rodata string
        ("rip", 0x7af0f1, 7, 3, 0x15ce260),  # functor vtable tag
        ("rip", 0x7af0f8, 7, 3, 0x1286db4),  # rodata string
        ("imm", 0x7af109, "41b901000000"),
        ("imm", 0x7af129, "48c744242821000000"),
        ("imm", 0x7af13c, "48c784249000000000000000"),
        ("imm", 0x7af15b, "c744241ccdcccc3d"),
        ("imm", 0x7af163, "c744241800008040"),
        ("imm", 0x7af16e, "48c78424a800000000000000"),
        ("imm", 0x7af191, "48c744245800000000"),
        ("call", 0x7af19d, 0xc300),
        ("call", 0x7af1a9, 0xf590),
        ("imm", 0x7af1be, "ff5018"),
        ("imm", 0x7af1c7, "c783d00000000000003f"),
        ("imm", 0x7af1d1, "c743680000003f"),
        ("call", 0x7af1d8, 0x20d60),
        ("call", 0x7af1e0, 0xf590),
        ("rip", 0x7af1e5, 7, 3, 0x20f0),  # destructor/function RVA
        ("rip", 0x7af1ec, 7, 3, 0x1604218),  # dso handle
        ("call", 0x7af1f6, 0xf8f390),
        ("rip", 0x7af1fb, 7, 3, 0x325b688),  # bss object/cell
        ("rip", 0x7af202, 7, 3, 0x1286d82),  # rodata string
        ("imm", 0x7af225, "b901000000"),
        ("imm", 0x7af22a, "48c784249000000000000000"),
        ("rip", 0x7af243, 7, 3, 0x15ce260),  # functor vtable tag
        ("imm", 0x7af250, "48c74424281e000000"),
        ("imm", 0x7af26b, "48c744245800000000"),
        ("imm", 0x7af27c, "48c78424a800000000000000"),
        ("call", 0x7af295, 0x6aa00),
        ("call", 0x7af29d, 0xf590),
        ("imm", 0x7af2ad, "ff5018"),
        ("imm", 0x7af2b6, "c783d00000000000803f"),
        ("imm", 0x7af2c0, "c743680000803f"),
        ("call", 0x7af2c7, 0x20d60),
        ("call", 0x7af2cf, 0xf590),
        ("rip", 0x7af2d4, 7, 3, 0x20f0),  # destructor/function RVA
        ("rip", 0x7af2db, 7, 3, 0x1604218),  # dso handle
        ("call", 0x7af2e5, 0xf8f390),
        ("rip", 0x7af2ea, 7, 3, 0x15fe518),  # stack canary cell
        ("call", 0x7af30d, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_350_7af0b0.hpp', 'source': 'native/core/ctor_slot_350_7af0b0.cpp', 'test': 'native/tests/test_ctor_slot_350.cpp', 'shapes_h': 'native/core/ctor_shapes_343_356.hpp', 'shapes_c': 'native/core/ctor_shapes_343_356.cpp', 'cmake': 'native/CMakeLists.txt', 'consts': {'kSlot350Rva': 8057008, 'kSlot350Size': 610}, 'header_tokens': ['0x3dcccccdu', '0x40800000u', '0x3f000000u', '0x3f800000u', 'kFloatArgFnVtbl_350', 'false,'], 'source_order': ['init_float_var_static(objects.var_a, kSlot350SpecA, concmd)', 'init_float_var_static(objects.var_b, kSlot350SpecB, concmd)'], 'test_tokens': ['kSlot350Rva == 0x7af0b0', 'section_alone()', 'section_failures()'], 'shapes_tokens': ['init_bounded_float_var_c300(var, name, &var.value, lo, hi, spec.flags, args, description)', 'init_unbounded_float_var_6aa00(var, name, &var.value, spec.flags, args, description)'], 'cmake_tokens': ['core/ctor_slot_350_7af0b0.cpp', 'add_executable(test_ctor_slot_350 tests/test_ctor_slot_350.cpp)', 'add_test(NAME ctor_slot_350 COMMAND test_ctor_slot_350)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Two float variables with functor 0x15ce260: bounded `nico_PhotoModeLwsZoomControlSpeed` (0xc300) and unbounded `shadow_bowZoomScaleOnHorseback` (0x6aa00).'))
