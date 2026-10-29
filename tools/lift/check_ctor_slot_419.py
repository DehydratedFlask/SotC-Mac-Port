#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 419 (RVA 0x56fae0, 402 bytes), lane M2.

Five 0x28-byte splash-screen records at 0x2f117b0 (immediate stores + five `$/CP3Shadow/BP_SplashScreens/_ps4/*.ctxr` path templates), then the tail `jmp 0x323d0` (TSC stamp into 0x2f11878).

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

lib.COMMON_CALLEES["store_tsc"] = (0x323D0, 67, "32da90bc4ce93a445148b51b3131ce7ca4ef6a5c88116c4c80cf0b87a437f70c")


SPEC = {
    "slot": 419,
    "base": 0x56fae0,
    "size": 402,
    "body_sha256": "721acbceeb25fb9b792736f316d834a481cd1215a5e91cabb1826a1b915d2c0c",
    "pad": "9090909090909090909090909090",
    "callees": ['store_tsc'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0x101eaf7, b'$/CP3Shadow/BP_SplashScreens/_ps4/epilepsy_%s%s.ctxr'),
        (0x101eb2c, b'$/CP3Shadow/BP_SplashScreens/_ps4/legal_jp_%s%s.ctxr'),
        (0x101eb61, b'$/CP3Shadow/BP_SplashScreens/_ps4/sce_presents_%s%s.ctxr'),
        (0x101eb9a, b'$/CP3Shadow/BP_SplashScreens/_ps4/JAPANstudio_%s%s.ctxr'),
        (0x101ebd2, b'$/CP3Shadow/BP_SplashScreens/_ps4/bluepoint_%s%s.ctxr'),
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x56fae0, 7, 3, 0x101eaf7),
        ("rip", 0x56fae7, 7, 3, 0x101eb2c),
        ("rip", 0x56faee, 7, 3, 0x101eb61),
        ("rip", 0x56faf5, 10, 2, 0x2f117b0),
        ("imm", 0x56faf5, "c705b11c9a020000a040"),
        ("rip", 0x56faff, 10, 2, 0x2f117b4),
        ("imm", 0x56faff, "c705ab1c9a020000003f"),
        ("rip", 0x56fb09, 10, 2, 0x2f117b8),
        ("imm", 0x56fb09, "c705a51c9a0200000000"),
        ("rip", 0x56fb13, 7, 2, 0x2f117bc),
        ("imm", 0x56fb13, "c605a21c9a0200"),
        ("rip", 0x56fb1a, 7, 2, 0x2f117bd),
        ("imm", 0x56fb1a, "c6059c1c9a0200"),
        ("rip", 0x56fb21, 10, 2, 0x2f117c0),
        ("imm", 0x56fb21, "c705951c9a0200000000"),
        ("rip", 0x56fb2b, 7, 3, 0x2f11878),
        ("rip", 0x56fb32, 7, 3, 0x2f117c8),
        ("imm", 0x56fb32, "4889058f1c9a02"),
        ("rip", 0x56fb39, 10, 2, 0x2f117d0),
        ("imm", 0x56fb39, "c7058d1c9a02ffffffff"),
        ("rip", 0x56fb43, 10, 2, 0x2f117d8),
        ("imm", 0x56fb43, "c7058b1c9a020000a040"),
        ("rip", 0x56fb4d, 10, 2, 0x2f117dc),
        ("imm", 0x56fb4d, "c705851c9a020000003f"),
        ("rip", 0x56fb57, 10, 2, 0x2f117e0),
        ("imm", 0x56fb57, "c7057f1c9a02abaaaa3e"),
        ("rip", 0x56fb61, 7, 2, 0x2f117e4),
        ("imm", 0x56fb61, "c6057c1c9a0200"),
        ("rip", 0x56fb68, 7, 2, 0x2f117e5),
        ("imm", 0x56fb68, "c605761c9a0200"),
        ("rip", 0x56fb6f, 10, 2, 0x2f117e8),
        ("imm", 0x56fb6f, "c7056f1c9a0200000000"),
        ("rip", 0x56fb79, 7, 3, 0x2f117f0),
        ("imm", 0x56fb79, "488915701c9a02"),
        ("rip", 0x56fb80, 10, 2, 0x2f117f8),
        ("imm", 0x56fb80, "c7056e1c9a02ffffffff"),
        ("rip", 0x56fb8a, 10, 2, 0x2f11800),
        ("imm", 0x56fb8a, "c7056c1c9a0200000040"),
        ("rip", 0x56fb94, 10, 2, 0x2f11804),
        ("imm", 0x56fb94, "c705661c9a020000003f"),
        ("rip", 0x56fb9e, 10, 2, 0x2f11808),
        ("imm", 0x56fb9e, "c705601c9a02abaaaa3e"),
        ("rip", 0x56fba8, 7, 2, 0x2f1180c),
        ("imm", 0x56fba8, "c6055d1c9a0200"),
        ("rip", 0x56fbaf, 7, 2, 0x2f1180d),
        ("imm", 0x56fbaf, "c605571c9a0201"),
        ("rip", 0x56fbb6, 10, 2, 0x2f11810),
        ("imm", 0x56fbb6, "c705501c9a0200000000"),
        ("rip", 0x56fbc0, 7, 3, 0x2f11818),
        ("imm", 0x56fbc0, "48890d511c9a02"),
        ("rip", 0x56fbc7, 7, 3, 0x101eb9a),
        ("rip", 0x56fbce, 7, 3, 0x101ebd2),
        ("rip", 0x56fbd5, 10, 2, 0x2f11820),
        ("imm", 0x56fbd5, "c705411c9a02ffffffff"),
        ("rip", 0x56fbdf, 10, 2, 0x2f11828),
        ("imm", 0x56fbdf, "c7053f1c9a0200000040"),
        ("rip", 0x56fbe9, 10, 2, 0x2f1182c),
        ("imm", 0x56fbe9, "c705391c9a020000003f"),
        ("rip", 0x56fbf3, 10, 2, 0x2f11830),
        ("imm", 0x56fbf3, "c705331c9a02abaaaa3e"),
        ("rip", 0x56fbfd, 7, 2, 0x2f11834),
        ("imm", 0x56fbfd, "c605301c9a0200"),
        ("rip", 0x56fc04, 7, 2, 0x2f11835),
        ("imm", 0x56fc04, "c6052a1c9a0201"),
        ("rip", 0x56fc0b, 10, 2, 0x2f11838),
        ("imm", 0x56fc0b, "c705231c9a0200000000"),
        ("rip", 0x56fc15, 7, 3, 0x2f11840),
        ("imm", 0x56fc15, "488915241c9a02"),
        ("rip", 0x56fc1c, 10, 2, 0x2f11848),
        ("imm", 0x56fc1c, "c705221c9a02ffffffff"),
        ("rip", 0x56fc26, 10, 2, 0x2f11850),
        ("imm", 0x56fc26, "c705201c9a0200000040"),
        ("rip", 0x56fc30, 10, 2, 0x2f11854),
        ("imm", 0x56fc30, "c7051a1c9a0200000000"),
        ("rip", 0x56fc3a, 10, 2, 0x2f11858),
        ("imm", 0x56fc3a, "c705141c9a02abaaaa3e"),
        ("rip", 0x56fc44, 7, 2, 0x2f1185c),
        ("imm", 0x56fc44, "c605111c9a0200"),
        ("rip", 0x56fc4b, 7, 2, 0x2f1185d),
        ("imm", 0x56fc4b, "c6050b1c9a0201"),
        ("rip", 0x56fc52, 10, 2, 0x2f11860),
        ("imm", 0x56fc52, "c705041c9a0200000000"),
        ("rip", 0x56fc5c, 7, 3, 0x2f11868),
        ("imm", 0x56fc5c, "48890d051c9a02"),
        ("rip", 0x56fc63, 10, 2, 0x2f11870),
        ("imm", 0x56fc63, "c705031c9a02ffffffff"),
        ("jmp", 0x56fc6d, 0x323d0),
    ],
    "native": {'header': 'native/core/ctor_slot_419_56fae0.hpp',
     'source': 'native/core/ctor_slot_419_56fae0.cpp',
     'test': 'native/tests/test_ctor_slot_419.cpp',
     'cmake': 'native/lanes/m2.cmake',
     'consts': {'kSlot419Rva': 5700320, 'kSlot419Size': 402},
     'header_tokens': ['kSlot419TableRva = 0x2f117b0', 'kSlot419TscRva = 0x2f11878', 'kSlot419RecordSize = 0x28'],
     'source_order': ['kF5 = 0x40a00000u',
                      'kF2 = 0x40000000u',
                      'kHalf = 0x3f000000u',
                      'kThird = 0x3eaaaaabu',
                      'store_record(o.records[0], kF5, kHalf, 0, 0, 0, kSlot419Path0)',
                      'store_record(o.records[1], kF5, kHalf, kThird, 0, 0, kSlot419Path1)',
                      'store_record(o.records[2], kF2, kHalf, kThird, 0, 1, kSlot419Path2)',
                      'store_record(o.records[3], kF2, kHalf, kThird, 0, 1, kSlot419Path3)',
                      'store_record(o.records[4], kF2, 0, kThird, 0, 1, kSlot419Path4)',
                      'store_tsc_323d0(&o.tsc, tsc_hooks)'],
     'test_tokens': ['kSlot419Rva == 0x56fae0', 'section_alone()', 'section_tsc_default_hooks()'],
     'cmake_tokens': ['core/ctor_slot_419_56fae0.cpp',
                      'add_executable(test_ctor_slot_419 tests/test_ctor_slot_419.cpp)',
                      'add_test(NAME ctor_slot_419 COMMAND test_ctor_slot_419)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Five 0x28-byte splash-screen records (immediate stores) + tail jmp to the 0x323d0 TSC stamp.'))
