#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 426 (RVA 0x53fb50, 336 bytes), lane m3.

Bool console variable `showGameStateValues` (object 0x2eff370, value 0, default 0, functor tag 0x15c8e98) and one zeroed 16-byte static (0x2eff438)
with __cxa_atexit(0x539a00, cell, &dso) (a shared-pointer-like destructor, hash-pinned, recorded, never run).

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

# Callees specific to this slot (rva, size, sha256), registered next to the shared L4 table.
lib.COMMON_CALLEES['pair_dtor_539a00'] = (0x539a00, 0x53, "aff9c9eb466ddc98ea5c5c3afb82efd3210696ab537960b0c27acc1247001f1d")

SPEC = {
    "slot": 426,
    "base": 0x53fb50,
    "size": 336,
    "body_sha256": "8be161c75d7642975a3932c9e9f99a0f66b754f34da8a57aa170811f955f3bb5",
    "pad": "90909090909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'pair_dtor_539a00'],
    "bool_tags": [0x15c8e98],
    "float_tags": [],
    "strings": [
        (0x10157e0, b'showGameStateValues'),
        (0x10168f2, b''),
    ],
    "lengths": [
        ('showGameStateValues', 0x53fbb2, "48c7459813000000"),
    ],
    "pins": [
        ("rip", 0x53fb61, 7, 3, 0x15fe518),
        ("rip", 0x53fb68, 7, 3, 0x1594208),
        ("rip", 0x53fb6f, 7, 3, 0x2eff370),
        ("rip", 0x53fb76, 7, 3, 0x10157e0),
        ("rip", 0x53fb7d, 7, 3, 0x10168f2),
        ("imm", 0x53fb90, "b901000000"),
        ("imm", 0x53fbb2, "48c7459813000000"),
        ("imm", 0x53fbc7, "48c745c800000000"),
        ("call", 0x53fbcf, 0x20af0),
        ("rip", 0x53fbd4, 7, 3, 0x15940a0),
        ("rip", 0x53fbe9, 7, 3, 0x15c8e98),
        ("imm", 0x53fbfb, "c7436900000000"),
        ("imm", 0x53fc07, "48c783a000000000000000"),
        ("imm", 0x53fc19, "48c783b800000000000000"),
        ("call", 0x53fc24, 0xf590),
        ("imm", 0x53fc2f, "c683c000000000"),
        ("imm", 0x53fc36, "c6436800"),
        ("call", 0x53fc3a, 0x20d60),
        ("call", 0x53fc42, 0xf590),
        ("rip", 0x53fc47, 7, 3, 0x1604218),
        ("rip", 0x53fc4e, 7, 3, 0x20b0),
        ("call", 0x53fc5b, 0xf8f390),
        ("rip", 0x53fc60, 7, 3, 0x539a00),
        ("rip", 0x53fc6b, 7, 3, 0x2eff438),
        ("rip", 0x53fc75, 8, 4, 0x2eff438),
        ("imm", 0x53fc75, "c5f81105bbf79b02"),
        ("call", 0x53fc7d, 0xf8f390),
        ("call", 0x53fc9b, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_426_53fb50.hpp', 'source': 'native/core/ctor_slot_426_53fb50.cpp', 'test': 'native/tests/test_ctor_slot_426.cpp', 'cmake': 'native/lanes/m3.cmake', 'consts': {'kSlot426Rva': 5503824, 'kSlot426Size': 336, 'kSlot426PairDtorRva': 5478912}, 'header_tokens': ['kSlot426FnTag = 0x15c8e98', 'kSlot426ObjectRva = 0x2eff370', 'kSlot426CellRva = 0x2eff438'], 'source_order': ['cell.ptr = 0;', 'cell.ctl = 0;', 'record(kSlot426PairDtorRva, &cell)', 'init_bool_var_site(objects.var, kSlot426Name, kSlot426FnTag, concmd, 0, 0)', 'init_slot426_pair16(objects.cell, concmd)'], 'test_tokens': ['kSlot426Rva == 0x53fb50', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_426_53fb50.cpp', 'add_executable(test_ctor_slot_426 tests/test_ctor_slot_426.cpp)', 'add_test(NAME ctor_slot_426 COMMAND test_ctor_slot_426)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'bool variable `showGameStateValues` (value 0, tag 0x15c8e98) + zeroed 16-byte static with atexit(0x539a00).'))
