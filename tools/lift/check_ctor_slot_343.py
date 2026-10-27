#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 343 (RVA 0x7d7970, 378 bytes), lane L4.

Two zeroed 24-byte statics with atexit(0x7d3af0) and the console command `fireCaption` (flags 2, handler tag 0x7d6420, atexit 0x4b0).

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
    "slot": 343,
    "base": 0x7d7970,
    "size": 378,
    "body_sha256": "ef04bcbf9a1d61d07775f3c71b61b297da2309b4337996981ba6d0cd49d9f59b",
    "pad": "909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'cmd_dtor', 'release_f590', 'zero_vector_dtor'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0x1288c44, b'fireCaption'),
        (0x1288c50, b'Fires any caption by name. Params: <caption name>'),
    ],
    "lengths": [
        ('Fires any caption by name. Params: <caption name>', 0x7d7a2b, "48c744243831000000"),
    ],
    "pins": [
        ("rip", 0x7d7984, 7, 3, 0x15fe518),  # stack canary cell
        ("rip", 0x7d798b, 7, 3, 0x7d3af0),  #
        ("rip", 0x7d7992, 7, 3, 0x1604218),  # dso handle
        ("rip", 0x7d7999, 7, 3, 0x325eb28),  # bss object/cell
        ("imm", 0x7d79b6, "48c7461000000000"),
        ("call", 0x7d79be, 0xf8f390),
        ("rip", 0x7d79c3, 7, 3, 0x325eb48),  # bss object/cell
        ("imm", 0x7d79d8, "48c7461000000000"),
        ("call", 0x7d79e0, 0xf8f390),
        ("rip", 0x7d79ea, 7, 3, 0x1288c44),  # rodata string
        ("call", 0x7d79f6, 0xf2f0),
        ("rip", 0x7d79fb, 7, 3, 0x325eb70),  # bss object/cell
        ("rip", 0x7d7a02, 8, 4, 0x15fe5e0),  #
        ("rip", 0x7d7a0a, 7, 3, 0x1288c50),  # rodata string
        ("imm", 0x7d7a1a, "b902000000"),
        ("imm", 0x7d7a2b, "48c744243831000000"),
        ("rip", 0x7d7a3e, 7, 3, 0x7d6420),  #
        ("call", 0x7d7a53, 0x20af0),
        ("rip", 0x7d7a58, 7, 3, 0x1594dc0),  # class vtable
        ("imm", 0x7d7a98, "48c7839000000000000000"),
        ("imm", 0x7d7aa3, "ff5020"),
        ("call", 0x7d7aab, 0xf590),
        ("call", 0x7d7ab5, 0xf590),
        ("rip", 0x7d7aba, 7, 3, 0x4b0),  # destructor/function RVA
        ("rip", 0x7d7ac1, 7, 3, 0x1604218),  # dso handle
        ("call", 0x7d7acb, 0xf8f390),
        ("call", 0x7d7ae5, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_343_7d7970.hpp', 'source': 'native/core/ctor_slot_343_7d7970.cpp', 'test': 'native/tests/test_ctor_slot_343.cpp', 'shapes_h': 'native/core/ctor_shapes_343_356.hpp', 'shapes_c': 'native/core/ctor_shapes_343_356.cpp', 'cmake': 'native/CMakeLists.txt', 'consts': {'kSlot343Rva': 8223088, 'kSlot343Size': 378}, 'header_tokens': ['kSlot343HandlerTag = 0x7d6420', 'kSlot343CommandDescriptionLength = 0x31'], 'source_order': ['init_zero_vector24(objects.cell_a, concmd)', 'init_zero_vector24(objects.cell_b, concmd)', 'init_fconcmd_object(objects.command, spec, concmd)'], 'test_tokens': ['kSlot343Rva == 0x7d7970', 'section_alone()', 'section_failures()'], 'shapes_tokens': ['cell.begin = 0;', 'record(kZeroVectorDtorRva, &cell)'], 'cmake_tokens': ['core/ctor_slot_343_7d7970.cpp', 'add_executable(test_ctor_slot_343 tests/test_ctor_slot_343.cpp)', 'add_test(NAME ctor_slot_343 COMMAND test_ctor_slot_343)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Two zeroed 24-byte statics with atexit(0x7d3af0) and the console command `fireCaption` (flags 2, handler tag 0x7d6420, atexit 0x4b0).'))
