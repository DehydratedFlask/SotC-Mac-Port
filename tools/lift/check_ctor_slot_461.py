#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 461 (RVA 0x419480, 323 bytes), lane m3.

One bool console variable `drawCameraControllerInfo` (object 0x2ef7bf0, value 1, default 1, functor tag 0x15b5198). Same stores as slot 348, rip-relative encoding.

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
    "slot": 461,
    "base": 0x419480,
    "size": 323,
    "body_sha256": "f4ac8a794a5f72437707edb5ceeaf38954998bc1a7c08f771496f16cd49f4755",
    "pad": "90909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15b5198],
    "float_tags": [],
    "strings": [
        (0xff0224, b'drawCameraControllerInfo'),
        (0xff03b0, b''),
    ],
    "lengths": [
        ('drawCameraControllerInfo', 0x4194e2, "48c7459818000000"),
    ],
    "pins": [
        ("rip", 0x419491, 7, 3, 0x15fe518),
        ("rip", 0x419498, 7, 3, 0x1594208),
        ("rip", 0x41949f, 7, 3, 0x2ef7bf0),
        ("rip", 0x4194a6, 7, 3, 0xff0224),
        ("rip", 0x4194ad, 7, 3, 0xff03b0),
        ("imm", 0x4194c0, "b901000000"),
        ("imm", 0x4194e2, "48c7459818000000"),
        ("rip", 0x4194f4, 7, 3, 0x2ef7bf0),
        ("imm", 0x4194fb, "48c745c800000000"),
        ("call", 0x419503, 0x20af0),
        ("rip", 0x419508, 7, 3, 0x15940a0),
        ("rip", 0x419512, 7, 3, 0x2ef7cb0),
        ("rip", 0x41951d, 7, 3, 0x15b5198),
        ("rip", 0x419528, 7, 3, 0x2ef7bf0),
        ("rip", 0x41952f, 7, 3, 0x2ef7c50),
        ("rip", 0x419536, 10, 2, 0x2ef7c59),
        ("imm", 0x419536, "c70519e7ad0200000000"),
        ("rip", 0x419540, 8, 4, 0x2ef7c60),
        ("imm", 0x419540, "c5fc110518e7ad02"),
        ("rip", 0x419548, 11, 3, 0x2ef7c90),
        ("imm", 0x419548, "48c7053de7ad0200000000"),
        ("rip", 0x419553, 7, 3, 0x2ef7ca0),
        ("rip", 0x41955a, 11, 3, 0x2ef7ca8),
        ("imm", 0x41955a, "48c70543e7ad0200000000"),
        ("call", 0x419565, 0xf590),
        ("rip", 0x41956d, 7, 3, 0x2ef7bf0),
        ("rip", 0x419574, 7, 2, 0x2ef7cb0),
        ("imm", 0x419574, "c60535e7ad0201"),
        ("rip", 0x41957b, 7, 2, 0x2ef7c58),
        ("imm", 0x41957b, "c605d6e6ad0201"),
        ("call", 0x419582, 0x20d60),
        ("call", 0x41958a, 0xf590),
        ("rip", 0x41958f, 7, 3, 0x20b0),
        ("rip", 0x419596, 7, 3, 0x1604218),
        ("call", 0x4195a0, 0xf8f390),
        ("call", 0x4195be, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_461_419480.hpp', 'source': 'native/core/ctor_slot_461_419480.cpp', 'test': 'native/tests/test_ctor_slot_461.cpp', 'cmake': 'native/lanes/m3.cmake', 'consts': {'kSlot461Rva': 4297856, 'kSlot461Size': 323}, 'header_tokens': ['kSlot461FnTag = 0x15b5198', 'kSlot461ObjectRva = 0x2ef7bf0'], 'source_order': ['init_bool_var_site(objects.var, kSlot461Name, kSlot461FnTag, concmd, 1, 1)'], 'test_tokens': ['kSlot461Rva == 0x419480', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_461_419480.cpp', 'add_executable(test_ctor_slot_461 tests/test_ctor_slot_461.cpp)', 'add_test(NAME ctor_slot_461 COMMAND test_ctor_slot_461)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'One bool console variable `drawCameraControllerInfo` (value 1, default 1, functor tag 0x15b5198, flags 1): 0x20af0, f590(desc), 0x20d60, f590(name), atexit(0x20b0).'))
