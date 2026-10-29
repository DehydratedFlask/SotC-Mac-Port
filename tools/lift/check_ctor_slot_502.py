#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 502 (RVA 0x249270, 323 bytes), lane M5.

Bool variable `r_showProjectedDecals` (value 0 / default 0, tag 0x159bc00, empty description), atexit 0x20b0.

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
    "slot": 502,
    "base": 0x249270,
    "size": 323,
    "body_sha256": "e7122922b4c440a2da47b691b8dd4da75982ea3c153ade92b25898871578425c",
    "pad": "90909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [22658048],
    "float_tags": [],
    "strings": [
        (0xfc2830, b'r_showProjectedDecals'),
        (0xfc285e, b''),
    ],
    "lengths": [
        ('r_showProjectedDecals', 0x2492d2, "48c7459815000000"),
    ],
    "pins": [
        ("rip", 0x249281, 7, 3, 0x15fe518),
        ("rip", 0x249288, 7, 3, 0x1594208),
        ("rip", 0x24928f, 7, 3, 0x2eeae10),
        ("rip", 0x249296, 7, 3, 0xfc2830),
        ("rip", 0x24929d, 7, 3, 0xfc285e),
        ("imm", 0x2492b0, "b901000000"),
        ("imm", 0x2492bc, "4883c310"),
        ("imm", 0x2492d2, "48c7459815000000"),
        ("rip", 0x2492e4, 7, 3, 0x2eeae10),
        ("imm", 0x2492eb, "48c745c800000000"),
        ("call", 0x2492f3, 0x20af0),
        ("rip", 0x2492f8, 7, 3, 0x15940a0),
        ("rip", 0x249302, 7, 3, 0x2eeaed0),
        ("rip", 0x24930d, 7, 3, 0x159bc00),
        ("imm", 0x249314, "4883c010"),
        ("rip", 0x249318, 7, 3, 0x2eeae10),
        ("rip", 0x24931f, 7, 3, 0x2eeae70),
        ("rip", 0x249326, 10, 2, 0x2eeae79),
        ("rip", 0x249330, 8, 4, 0x2eeae80),
        ("rip", 0x249338, 11, 3, 0x2eeaeb0),
        ("rip", 0x249343, 7, 3, 0x2eeaec0),
        ("rip", 0x24934a, 11, 3, 0x2eeaec8),
        ("call", 0x249355, 0xf590),
        ("rip", 0x24935d, 7, 3, 0x2eeae10),
        ("rip", 0x249364, 7, 2, 0x2eeaed0),
        ("rip", 0x24936b, 7, 2, 0x2eeae78),
        ("call", 0x249372, 0x20d60),
        ("call", 0x24937a, 0xf590),
        ("rip", 0x24937f, 7, 3, 0x20b0),
        ("rip", 0x249386, 7, 3, 0x1604218),
        ("call", 0x249390, 0xf8f390),
        ("imm", 0x24939f, "4883c448"),
        ("call", 0x2493ae, 0xf8f3a0),
    ],
    "elf_extra": [],
    "native": {'header': 'native/core/ctor_slot_502_249270.hpp',
     'source': 'native/core/ctor_slot_502_249270.cpp',
     'test': 'native/tests/test_ctor_slot_502.cpp',
     'cmake': 'native/lanes/m5.cmake',
     'consts': {'kSlot502Rva': 2396784, 'kSlot502Size': 323},
     'header_tokens': ['kSlot502FnTag = 0x159bc00'],
     'source_order': ['init_bool_var_site(objects.var, kSlot502Name, kSlot502FnTag, concmd, 0, 0)'],
     'test_tokens': ['kSlot502Rva == 0x249270', 'section_alone()', 'section_failures()'],
     'cmake_tokens': ['core/ctor_slot_502_249270.cpp',
                      'add_executable(test_ctor_slot_502 tests/test_ctor_slot_502.cpp)',
                      'add_test(NAME ctor_slot_502 COMMAND test_ctor_slot_502)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bool variable `r_showProjectedDecals` (value 0 / default 0, tag 0x159bc00, empty description), atexit 0x20b0.'))
