#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 441 (RVA 0x4e68c0, 410 bytes), lane m3.

Bool console variable `cp10_debugSwordLight` (object 0x2efbf08, value 0, default 0, functor tag 0x15c2c20) and a debug-menu toggle built by 0x26db0
(item 0x2efbfd0) under the parent path `Shadow Player`.

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
    "slot": 441,
    "base": 0x4e68c0,
    "size": 410,
    "body_sha256": "8238eb7bfd70b890c6a77ee61c37a52c65859b55504daa38a37328340e35312a",
    "pad": "909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_from_var_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15c2c20],
    "float_tags": [],
    "strings": [
        (0x10092f8, b'cp10_debugSwordLight'),
        (0x100935e, b''),
        (0x100930d, b'Shadow Player'),
    ],
    "lengths": [
        ('cp10_debugSwordLight', 0x4e6922, "48c7459814000000"),
        ('Shadow Player', 0x4e6a10, "48c745b80d000000"),
    ],
    "pins": [
        ("rip", 0x4e68d1, 7, 3, 0x15fe518),
        ("rip", 0x4e68d8, 7, 3, 0x1594208),
        ("rip", 0x4e68df, 7, 3, 0x2efbf08),
        ("rip", 0x4e68e6, 7, 3, 0x10092f8),
        ("rip", 0x4e68ed, 7, 3, 0x100935e),
        ("imm", 0x4e6900, "b901000000"),
        ("imm", 0x4e6922, "48c7459814000000"),
        ("rip", 0x4e6934, 7, 3, 0x2efbf08),
        ("imm", 0x4e693b, "48c745c800000000"),
        ("call", 0x4e6943, 0x20af0),
        ("rip", 0x4e6948, 7, 3, 0x15940a0),
        ("rip", 0x4e6952, 7, 3, 0x2efbfc8),
        ("rip", 0x4e695d, 7, 3, 0x15c2c20),
        ("rip", 0x4e6968, 7, 3, 0x2efbf08),
        ("rip", 0x4e696f, 7, 3, 0x2efbf68),
        ("rip", 0x4e6976, 10, 2, 0x2efbf71),
        ("imm", 0x4e6976, "c705f155a10200000000"),
        ("rip", 0x4e6980, 8, 4, 0x2efbf78),
        ("imm", 0x4e6980, "c5fc1105f055a102"),
        ("rip", 0x4e6988, 11, 3, 0x2efbfa8),
        ("imm", 0x4e6988, "48c7051556a10200000000"),
        ("rip", 0x4e6993, 7, 3, 0x2efbfb8),
        ("rip", 0x4e699a, 11, 3, 0x2efbfc0),
        ("imm", 0x4e699a, "48c7051b56a10200000000"),
        ("call", 0x4e69a5, 0xf590),
        ("rip", 0x4e69ad, 7, 3, 0x2efbf08),
        ("rip", 0x4e69b4, 7, 2, 0x2efbfc8),
        ("imm", 0x4e69b4, "c6050d56a10200"),
        ("rip", 0x4e69bb, 7, 2, 0x2efbf70),
        ("imm", 0x4e69bb, "c605ae55a10200"),
        ("call", 0x4e69c2, 0x20d60),
        ("call", 0x4e69ca, 0xf590),
        ("rip", 0x4e69cf, 7, 3, 0x1604218),
        ("rip", 0x4e69d6, 7, 3, 0x20b0),
        ("call", 0x4e69e3, 0xf8f390),
        ("rip", 0x4e69e8, 7, 3, 0x2efbfd0),
        ("rip", 0x4e69f3, 7, 3, 0x100930d),
        ("imm", 0x4e69fe, "b930000000"),
        ("imm", 0x4e6a10, "48c745b80d000000"),
        ("call", 0x4e6a1d, 0x26db0),
        ("call", 0x4e6a25, 0xf590),
        ("rip", 0x4e6a2a, 7, 3, 0x272d0),
        ("call", 0x4e6a37, 0xf8f390),
        ("call", 0x4e6a55, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_441_4e68c0.hpp', 'source': 'native/core/ctor_slot_441_4e68c0.cpp', 'test': 'native/tests/test_ctor_slot_441.cpp', 'cmake': 'native/lanes/m3.cmake', 'consts': {'kSlot441Rva': 5138624, 'kSlot441Size': 410}, 'header_tokens': ['kSlot441FnTag = 0x15c2c20', 'kSlot441ObjectRva = 0x2efbf08', 'kSlot441ItemRva = 0x2efbfd0'], 'source_order': ['init_bool_var_site(objects.var, kSlot441VarName, kSlot441FnTag, concmd, 0, 0)', 'init_toggle_site_26db0(objects.item, objects.var, kSlot441Parent, concmd, menu)'], 'test_tokens': ['kSlot441Rva == 0x4e68c0', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_441_4e68c0.cpp', 'add_executable(test_ctor_slot_441 tests/test_ctor_slot_441.cpp)', 'add_test(NAME ctor_slot_441 COMMAND test_ctor_slot_441)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'bool variable `cp10_debugSwordLight` (value 0, tag 0x15c2c20) + 0x26db0 toggle under `Shadow Player` (flags 0x30, f590(parent), atexit 0x272d0).'))
