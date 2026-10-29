#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 499 (RVA 0x2500e0, 323 bytes), lane M5.

Bool variable `r_showDirectionalBoxLights` (value 0 / default 0, tag 0x159c318, empty description), atexit 0x20b0.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and indirect call of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings and the hash of every shared callee. A mutation self-test flips the
first/last byte of every pin and perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lane_m5_check as lib  # noqa: E402

SPEC = {
    "slot": 499,
    "base": 0x2500e0,
    "size": 323,
    "body_sha256": "44dd64abae1eb2c1d88030615697f80b0e8002688cae358a751ae8562e6f2e0d",
    "pad": "90909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [22659864],
    "float_tags": [],
    "strings": [
        (0xfc2ce0, b'r_showDirectionalBoxLights'),
        (0xfc2d1a, b''),
    ],
    "lengths": [
        ('r_showDirectionalBoxLights', 0x250142, "48c745981a000000"),
    ],
    "pins": [
        ("rip", 0x2500f1, 7, 3, 0x15fe518),
        ("rip", 0x2500f8, 7, 3, 0x1594208),
        ("rip", 0x2500ff, 7, 3, 0x2eebc58),
        ("rip", 0x250106, 7, 3, 0xfc2ce0),
        ("rip", 0x25010d, 7, 3, 0xfc2d1a),
        ("imm", 0x250120, "b901000000"),
        ("imm", 0x25012c, "4883c310"),
        ("imm", 0x250142, "48c745981a000000"),
        ("rip", 0x250154, 7, 3, 0x2eebc58),
        ("imm", 0x25015b, "48c745c800000000"),
        ("call", 0x250163, 0x20af0),
        ("rip", 0x250168, 7, 3, 0x15940a0),
        ("rip", 0x250172, 7, 3, 0x2eebd18),
        ("rip", 0x25017d, 7, 3, 0x159c318),
        ("imm", 0x250184, "4883c010"),
        ("rip", 0x250188, 7, 3, 0x2eebc58),
        ("rip", 0x25018f, 7, 3, 0x2eebcb8),
        ("rip", 0x250196, 10, 2, 0x2eebcc1),
        ("rip", 0x2501a0, 8, 4, 0x2eebcc8),
        ("rip", 0x2501a8, 11, 3, 0x2eebcf8),
        ("rip", 0x2501b3, 7, 3, 0x2eebd08),
        ("rip", 0x2501ba, 11, 3, 0x2eebd10),
        ("call", 0x2501c5, 0xf590),
        ("rip", 0x2501cd, 7, 3, 0x2eebc58),
        ("rip", 0x2501d4, 7, 2, 0x2eebd18),
        ("rip", 0x2501db, 7, 2, 0x2eebcc0),
        ("call", 0x2501e2, 0x20d60),
        ("call", 0x2501ea, 0xf590),
        ("rip", 0x2501ef, 7, 3, 0x20b0),
        ("rip", 0x2501f6, 7, 3, 0x1604218),
        ("call", 0x250200, 0xf8f390),
        ("imm", 0x25020f, "4883c448"),
        ("call", 0x25021e, 0xf8f3a0),
    ],
    "elf_extra": [],
    "native": {'header': 'native/core/ctor_slot_499_2500e0.hpp',
     'source': 'native/core/ctor_slot_499_2500e0.cpp',
     'test': 'native/tests/test_ctor_slot_499.cpp',
     'cmake': 'native/lanes/m5.cmake',
     'consts': {'kSlot499Rva': 2425056, 'kSlot499Size': 323},
     'header_tokens': ['kSlot499FnTag = 0x159c318'],
     'source_order': ['init_bool_var_site(objects.var, kSlot499Name, kSlot499FnTag, concmd, 0, 0)'],
     'test_tokens': ['kSlot499Rva == 0x2500e0', 'section_alone()', 'section_failures()'],
     'cmake_tokens': ['core/ctor_slot_499_2500e0.cpp',
                      'add_executable(test_ctor_slot_499 tests/test_ctor_slot_499.cpp)',
                      'add_test(NAME ctor_slot_499 COMMAND test_ctor_slot_499)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bool variable `r_showDirectionalBoxLights` (value 0 / default 0, tag 0x159c318, empty description), atexit 0x20b0.'))
