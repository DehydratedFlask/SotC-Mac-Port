#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 456 (RVA 0x471950, 323 bytes), lane m3.

One bool console variable `wwise_postSounds` (object 0x2ef8188, value 1, default 1, functor tag 0x15bc748). Same stores as slot 348, rip-relative encoding.

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
    "slot": 456,
    "base": 0x471950,
    "size": 323,
    "body_sha256": "65e7bac2b56dc7229969b011db2a5f67026e4a3b97c7c691b0662c417a41683a",
    "pad": "90909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15bc748],
    "float_tags": [],
    "strings": [
        (0xff94c6, b'wwise_postSounds'),
        (0xff9674, b''),
    ],
    "lengths": [
        ('wwise_postSounds', 0x4719b2, "48c7459810000000"),
    ],
    "pins": [
        ("rip", 0x471961, 7, 3, 0x15fe518),
        ("rip", 0x471968, 7, 3, 0x1594208),
        ("rip", 0x47196f, 7, 3, 0x2ef8188),
        ("rip", 0x471976, 7, 3, 0xff94c6),
        ("rip", 0x47197d, 7, 3, 0xff9674),
        ("imm", 0x471990, "b901000000"),
        ("imm", 0x4719b2, "48c7459810000000"),
        ("rip", 0x4719c4, 7, 3, 0x2ef8188),
        ("imm", 0x4719cb, "48c745c800000000"),
        ("call", 0x4719d3, 0x20af0),
        ("rip", 0x4719d8, 7, 3, 0x15940a0),
        ("rip", 0x4719e2, 7, 3, 0x2ef8248),
        ("rip", 0x4719ed, 7, 3, 0x15bc748),
        ("rip", 0x4719f8, 7, 3, 0x2ef8188),
        ("rip", 0x4719ff, 7, 3, 0x2ef81e8),
        ("rip", 0x471a06, 10, 2, 0x2ef81f1),
        ("imm", 0x471a06, "c705e167a80200000000"),
        ("rip", 0x471a10, 8, 4, 0x2ef81f8),
        ("imm", 0x471a10, "c5fc1105e067a802"),
        ("rip", 0x471a18, 11, 3, 0x2ef8228),
        ("imm", 0x471a18, "48c7050568a80200000000"),
        ("rip", 0x471a23, 7, 3, 0x2ef8238),
        ("rip", 0x471a2a, 11, 3, 0x2ef8240),
        ("imm", 0x471a2a, "48c7050b68a80200000000"),
        ("call", 0x471a35, 0xf590),
        ("rip", 0x471a3d, 7, 3, 0x2ef8188),
        ("rip", 0x471a44, 7, 2, 0x2ef8248),
        ("imm", 0x471a44, "c605fd67a80201"),
        ("rip", 0x471a4b, 7, 2, 0x2ef81f0),
        ("imm", 0x471a4b, "c6059e67a80201"),
        ("call", 0x471a52, 0x20d60),
        ("call", 0x471a5a, 0xf590),
        ("rip", 0x471a5f, 7, 3, 0x20b0),
        ("rip", 0x471a66, 7, 3, 0x1604218),
        ("call", 0x471a70, 0xf8f390),
        ("call", 0x471a8e, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_456_471950.hpp', 'source': 'native/core/ctor_slot_456_471950.cpp', 'test': 'native/tests/test_ctor_slot_456.cpp', 'cmake': 'native/lanes/m3.cmake', 'consts': {'kSlot456Rva': 4659536, 'kSlot456Size': 323}, 'header_tokens': ['kSlot456FnTag = 0x15bc748', 'kSlot456ObjectRva = 0x2ef8188'], 'source_order': ['init_bool_var_site(objects.var, kSlot456Name, kSlot456FnTag, concmd, 1, 1)'], 'test_tokens': ['kSlot456Rva == 0x471950', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_456_471950.cpp', 'add_executable(test_ctor_slot_456 tests/test_ctor_slot_456.cpp)', 'add_test(NAME ctor_slot_456 COMMAND test_ctor_slot_456)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'One bool console variable `wwise_postSounds` (value 1, default 1, functor tag 0x15bc748, flags 1): 0x20af0, f590(desc), 0x20d60, f590(name), atexit(0x20b0).'))
