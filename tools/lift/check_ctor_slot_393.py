#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 393 (RVA 0x66cc00, 299 bytes), lane M2.

Single bool variable `cp10_debugDrawFruit` (functor tag 0x15cc5c8, value/default 0, flags 1).

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
    "slot": 393,
    "base": 0x66cc00,
    "size": 299,
    "body_sha256": "8ed706dcef75eee2eea86dd513262bc6e497bd0144b2635d42efd4aa6c88b148",
    "pad": "9090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15cc5c8],
    "float_tags": [],
    "strings": [
        (0x12701c8, b'cp10_debugDrawFruit'),
        (0x1270351, b''),
    ],
    "lengths": [
        ('cp10_debugDrawFruit', 0x66cc62, "48c7459813000000"),
    ],
    "pins": [
        ("rip", 0x66cc11, 7, 3, 0x15fe518),
        ("rip", 0x66cc18, 7, 3, 0x1594208),
        ("rip", 0x66cc1f, 7, 3, 0x3012648),
        ("rip", 0x66cc26, 7, 3, 0x12701c8),
        ("rip", 0x66cc2d, 7, 3, 0x1270351),
        ("imm", 0x66cc40, "b901000000"),
        ("imm", 0x66cc49, "4983c510"),
        ("imm", 0x66cc62, "48c7459813000000"),
        ("imm", 0x66cc74, "4c892b"),
        ("imm", 0x66cc77, "48c745c800000000"),
        ("call", 0x66cc7f, 0x20af0),
        ("rip", 0x66cc84, 7, 3, 0x15940a0),
        ("imm", 0x66cc8e, "488d93c0000000"),
        ("rip", 0x66cc99, 7, 3, 0x15cc5c8),
        ("imm", 0x66cca0, "4883c010"),
        ("imm", 0x66cca4, "488903"),
        ("imm", 0x66cca7, "48895360"),
        ("imm", 0x66ccab, "c7436900000000"),
        ("imm", 0x66ccb2, "c5fc114370"),
        ("imm", 0x66ccb7, "48c783a000000000000000"),
        ("imm", 0x66ccc2, "48898bb0000000"),
        ("imm", 0x66ccc9, "48c783b800000000000000"),
        ("call", 0x66ccd4, 0xf590),
        ("imm", 0x66ccdc, "4c892b"),
        ("imm", 0x66ccdf, "c683c000000000"),
        ("imm", 0x66cce6, "c6436800"),
        ("call", 0x66ccea, 0x20d60),
        ("call", 0x66ccf2, 0xf590),
        ("rip", 0x66ccf7, 7, 3, 0x20b0),
        ("rip", 0x66ccfe, 7, 3, 0x1604218),
        ("call", 0x66cd08, 0xf8f390),
        ("call", 0x66cd26, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_393_66cc00.hpp',
     'source': 'native/core/ctor_slot_393_66cc00.cpp',
     'test': 'native/tests/test_ctor_slot_393.cpp',
     'cmake': 'native/lanes/m2.cmake',
     'consts': {'kSlot393Rva': 6736896, 'kSlot393Size': 299},
     'header_tokens': ['kSlot393FnTag = 0x15cc5c8'],
     'source_order': ['init_bool_var_site(objects.var, kSlot393VarName, kSlot393FnTag, concmd, 0, 0)'],
     'test_tokens': ['kSlot393Rva == 0x66cc00', 'section_alone()', 'section_failures()'],
     'cmake_tokens': ['core/ctor_slot_393_66cc00.cpp',
                      'add_executable(test_ctor_slot_393 tests/test_ctor_slot_393.cpp)',
                      'add_test(NAME ctor_slot_393 COMMAND test_ctor_slot_393)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bool variable `cp10_debugDrawFruit` (tag 0x15cc5c8).'))
