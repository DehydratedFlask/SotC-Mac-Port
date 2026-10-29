#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 421 (RVA 0x56da20, 329 bytes), lane M2.

Bool variable `filesystem_allowAsync` (functor tag 0x15cade0, value/default 0, constructor flags 8) and a 128-byte zero block at 0x2f090d0 (no atexit).

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
    "slot": 421,
    "base": 0x56da20,
    "size": 329,
    "body_sha256": "05d9d3510fbab65d30a9dc9877ea7ce2d3f863233732497e22e7a170eca7455f",
    "pad": "90909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15cade0],
    "float_tags": [],
    "strings": [
        (0x101dbb0, b'filesystem_allowAsync'),
        (0x101e73a, b''),
    ],
    "lengths": [
        ('filesystem_allowAsync', 0x56da82, "48c7459815000000"),
    ],
    "pins": [
        ("rip", 0x56da31, 7, 3, 0x15fe518),
        ("rip", 0x56da38, 7, 3, 0x1594208),
        ("rip", 0x56da3f, 7, 3, 0x2f08fe8),
        ("rip", 0x56da46, 7, 3, 0x101dbb0),
        ("rip", 0x56da4d, 7, 3, 0x101e73a),
        ("imm", 0x56da60, "b908000000"),
        ("imm", 0x56da69, "4983c510"),
        ("imm", 0x56da82, "48c7459815000000"),
        ("imm", 0x56da94, "4c892b"),
        ("imm", 0x56da97, "48c745c800000000"),
        ("call", 0x56da9f, 0x20af0),
        ("rip", 0x56daa4, 7, 3, 0x15940a0),
        ("imm", 0x56daae, "488d93c0000000"),
        ("imm", 0x56dab5, "c5fc57c0"),
        ("rip", 0x56dab9, 7, 3, 0x15cade0),
        ("imm", 0x56dac0, "4883c010"),
        ("imm", 0x56dac4, "488903"),
        ("imm", 0x56dac7, "48895360"),
        ("imm", 0x56dacb, "c7436900000000"),
        ("imm", 0x56dad2, "c5fc114370"),
        ("imm", 0x56dad7, "48c783a000000000000000"),
        ("imm", 0x56dae2, "48898bb0000000"),
        ("imm", 0x56dae9, "48c783b800000000000000"),
        ("call", 0x56daf4, 0xf590),
        ("imm", 0x56dafc, "4c892b"),
        ("imm", 0x56daff, "c683c000000000"),
        ("imm", 0x56db06, "c6436800"),
        ("call", 0x56db0a, 0x20d60),
        ("call", 0x56db12, 0xf590),
        ("rip", 0x56db17, 7, 3, 0x20b0),
        ("rip", 0x56db1e, 7, 3, 0x1604218),
        ("call", 0x56db28, 0xf8f390),
        ("rip", 0x56db2d, 7, 3, 0x2f090d0),
        ("imm", 0x56db34, "c5fc57c0"),
        ("imm", 0x56db38, "c5fc114060"),
        ("imm", 0x56db3d, "c5fc114040"),
        ("imm", 0x56db42, "c5fc114020"),
        ("imm", 0x56db47, "c5fc1100"),
        ("call", 0x56db64, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_421_56da20.hpp',
     'source': 'native/core/ctor_slot_421_56da20.cpp',
     'test': 'native/tests/test_ctor_slot_421.cpp',
     'cmake': 'native/lanes/m2.cmake',
     'shapes_h': 'native/core/lane_m2_shapes.hpp',
     'shapes_c': 'native/core/lane_m2_shapes.cpp',
     'consts': {'kSlot421Rva': 5691936, 'kSlot421Size': 329},
     'header_tokens': ['kSlot421FnTag = 0x15cade0', 'kSlot421CtorFlags = 8'],
     'source_order': ['lane_m2::init_bool_var_flagged(objects.var, kSlot421VarName, kSlot421FnTag, kSlot421CtorFlags, concmd',
                      'std::memset(block + 0x60, 0, 0x20)',
                      'std::memset(block + 0x40, 0, 0x20)',
                      'std::memset(block + 0x20, 0, 0x20)',
                      'std::memset(block + 0x00, 0, 0x20)'],
     'shapes_tokens': ['init_concmd_base_20af0(var.base, n, d, flags)', 'record(kBoolVarDtorRva, &var)'],
     'test_tokens': ['kSlot421Rva == 0x56da20', 'section_alone()', 'section_flags_helper()', 'section_failures()'],
     'cmake_tokens': ['core/ctor_slot_421_56da20.cpp',
                      'add_executable(test_ctor_slot_421 tests/test_ctor_slot_421.cpp)',
                      'add_test(NAME ctor_slot_421 COMMAND test_ctor_slot_421)',
                      'core/lane_m2_shapes.cpp']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bool variable `filesystem_allowAsync` (flags 8, tag 0x15cade0) + 128-byte zero block.'))
