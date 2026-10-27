#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 348 (RVA 0x7c9150, 299 bytes), lane L4.

One bool console variable `cp3_shadow_enableDebugMove` (value 1, default 1, functor tag 0x15ce2f8, flags 1): 0x20af0, f590(desc), 0x20d60, f590(name), atexit(0x20b0).

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
    "slot": 348,
    "base": 0x7c9150,
    "size": 299,
    "body_sha256": "ef2ab8e100331e7335484156aad79de6f0cfc0d78007a5741020a1ad8360f155",
    "pad": "9090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15ce2f8],
    "float_tags": [],
    "strings": [
        (0x1287f14, b'cp3_shadow_enableDebugMove'),
        (0x1287f62, b''),
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x7c9161, 7, 3, 0x15fe518),  # stack canary cell
        ("rip", 0x7c9168, 7, 3, 0x1594208),  # class vtable
        ("rip", 0x7c916f, 7, 3, 0x325dd78),  # bss object/cell
        ("rip", 0x7c9176, 7, 3, 0x1287f14),  # rodata string
        ("rip", 0x7c917d, 7, 3, 0x1287f62),  # rodata string
        ("imm", 0x7c9190, "b901000000"),
        ("imm", 0x7c91b2, "48c745981a000000"),
        ("imm", 0x7c91c7, "48c745c800000000"),
        ("call", 0x7c91cf, 0x20af0),
        ("rip", 0x7c91d4, 7, 3, 0x15940a0),  # class vtable
        ("rip", 0x7c91e9, 7, 3, 0x15ce2f8),  # functor vtable tag
        ("imm", 0x7c91fb, "c7436900000000"),
        ("imm", 0x7c9207, "48c783a000000000000000"),
        ("imm", 0x7c9219, "48c783b800000000000000"),
        ("call", 0x7c9224, 0xf590),
        ("imm", 0x7c922f, "c683c000000001"),
        ("imm", 0x7c9236, "c6436801"),
        ("call", 0x7c923a, 0x20d60),
        ("call", 0x7c9242, 0xf590),
        ("rip", 0x7c9247, 7, 3, 0x20b0),  # destructor/function RVA
        ("rip", 0x7c924e, 7, 3, 0x1604218),  # dso handle
        ("call", 0x7c9258, 0xf8f390),
        ("call", 0x7c9276, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_348_7c9150.hpp', 'source': 'native/core/ctor_slot_348_7c9150.cpp', 'test': 'native/tests/test_ctor_slot_348.cpp', 'shapes_h': 'native/core/ctor_shapes_343_356.hpp', 'shapes_c': 'native/core/ctor_shapes_343_356.cpp', 'cmake': 'native/CMakeLists.txt', 'consts': {'kSlot348Rva': 8163664, 'kSlot348Size': 299}, 'header_tokens': ['kSlot348FnTag = 0x15ce2f8'], 'source_order': ['init_bool_var_site(objects.var, kSlot348Name, kSlot348FnTag, concmd, 1, 1)'], 'test_tokens': ['kSlot348Rva == 0x7c9150', 'section_alone()', 'section_failures()'], 'shapes_tokens': [], 'cmake_tokens': ['core/ctor_slot_348_7c9150.cpp', 'add_executable(test_ctor_slot_348 tests/test_ctor_slot_348.cpp)', 'add_test(NAME ctor_slot_348 COMMAND test_ctor_slot_348)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'One bool console variable `cp3_shadow_enableDebugMove` (value 1, default 1, functor tag 0x15ce2f8, flags 1): 0x20af0, f590(desc), 0x20d60, f590(name), atexit(0x20b0).'))
