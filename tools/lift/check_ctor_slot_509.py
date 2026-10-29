#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 509 (RVA 0x2196f0, 431 bytes), lane M5.

Bool variable `showBroadcastEvents` (value 0 / default 0, tag 0x159b170) plus a 0x26ea0 toggle (flags 0x30), text released before label, atexit 0x272d0.

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
    "slot": 509,
    "base": 0x2196f0,
    "size": 431,
    "body_sha256": "56ddf1eed2a77ba72a2d5758396de77021a03a76e978da607da2180f0197d23a",
    "pad": "90",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_ctor', 'menu_item_dtor'],
    "bool_tags": [22655344],
    "float_tags": [],
    "strings": [
        (0xfbf3d2, b'showBroadcastEvents'),
        (0xfbf60d, b''),
        (0xfbf3e6, b'Game Manager'),
        (0xfbf3f3, b'Show Broadcast Events'),
    ],
    "lengths": [
        ('showBroadcastEvents', 0x219751, "48c7459813000000"),
        ('Game Manager', 0x219833, "48c745b80c000000"),
        ('Show Broadcast Events', 0x219844, "48c7459815000000"),
    ],
    "pins": [
        ("rip", 0x219701, 7, 3, 0x15fe518),
        ("rip", 0x219708, 7, 3, 0x1594208),
        ("rip", 0x21970f, 7, 3, 0x2ee86b0),
        ("rip", 0x219716, 7, 3, 0xfbf3d2),
        ("rip", 0x21971d, 7, 3, 0xfbf60d),
        ("imm", 0x219730, "b901000000"),
        ("imm", 0x219738, "4983c410"),
        ("imm", 0x219751, "48c7459813000000"),
        ("imm", 0x219766, "48c745c800000000"),
        ("call", 0x21976e, 0x20af0),
        ("rip", 0x219773, 7, 3, 0x15940a0),
        ("rip", 0x219788, 7, 3, 0x159b170),
        ("imm", 0x21978f, "4883c010"),
        ("imm", 0x21979a, "c7436900000000"),
        ("imm", 0x2197a6, "48c783a000000000000000"),
        ("imm", 0x2197b8, "48c783b800000000000000"),
        ("call", 0x2197c3, 0xf590),
        ("imm", 0x2197ce, "c683c000000000"),
        ("imm", 0x2197d5, "c6436800"),
        ("call", 0x2197d9, 0x20d60),
        ("call", 0x2197e1, 0xf590),
        ("rip", 0x2197e6, 7, 3, 0x1604218),
        ("rip", 0x2197ed, 7, 3, 0x20b0),
        ("call", 0x2197fa, 0xf8f390),
        ("rip", 0x2197ff, 7, 3, 0x2ee8778),
        ("rip", 0x219806, 7, 3, 0xfbf3e6),
        ("imm", 0x219819, "41b830000000"),
        ("rip", 0x219826, 7, 3, 0xfbf3f3),
        ("imm", 0x219833, "48c745b80c000000"),
        ("imm", 0x219844, "48c7459815000000"),
        ("call", 0x219854, 0x26ea0),
        ("call", 0x21985c, 0xf590),
        ("call", 0x219864, 0xf590),
        ("rip", 0x219869, 7, 3, 0x272d0),
        ("call", 0x219876, 0xf8f390),
        ("rip", 0x21987b, 7, 3, 0x15fe518),
        ("imm", 0x21988b, "4883c448"),
        ("call", 0x21989a, 0xf8f3a0),
    ],
    "elf_extra": [],
    "native": {'header': 'native/core/ctor_slot_509_2196f0.hpp',
     'source': 'native/core/ctor_slot_509_2196f0.cpp',
     'test': 'native/tests/test_ctor_slot_509.cpp',
     'cmake': 'native/lanes/m5.cmake',
     'consts': {'kSlot509Rva': 2201328, 'kSlot509Size': 431},
     'header_tokens': ['kSlot509FnTag = 0x159b170', 'kSlot509ItemRva = 0x2ee8778'],
     'source_order': ['init_bool_var_site(objects.var, kSlot509Name, kSlot509FnTag, concmd, 0, 0)',
                      'init_toggle_site_26ea0(objects.item, objects.var, kSlot509Label, kSlot509Text, concmd, menu)'],
     'test_tokens': ['kSlot509Rva == 0x2196f0', 'section_alone()', 'section_failures()'],
     'cmake_tokens': ['core/ctor_slot_509_2196f0.cpp',
                      'add_executable(test_ctor_slot_509 tests/test_ctor_slot_509.cpp)',
                      'add_test(NAME ctor_slot_509 COMMAND test_ctor_slot_509)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bool variable `showBroadcastEvents` (value 0 / default 0, tag 0x159b170) plus a 0x26ea0 toggle (flags 0x30), text released before label, atexit 0x272d0.'))
