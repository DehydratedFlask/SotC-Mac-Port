#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 347 (RVA 0x7c93a0, 370 bytes), lane L4.

Bounded float variable `nico_AltDiveAttackHoldTime` through 0xc300 (min 0, max 10.0, default 0x3e19999a, functor 0x15ce318).

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
    "slot": 347,
    "base": 0x7c93a0,
    "size": 370,
    "body_sha256": "a90bbff0d94adeb997581b82a73f0b46da414f1cc038fbabdb593a0932301a78",
    "pad": "9090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'float_dtor', 'release_f590', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy', 'bounded_float_ctor'],
    "bool_tags": [],
    "float_tags": [0x15ce318],
    "strings": [
        (0x1287fee, b'nico_AltDiveAttackHoldTime'),
        (0x128800e, b''),
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x7c93b8, 7, 3, 0x15fe518),  # stack canary cell
        ("rip", 0x7c93bf, 7, 3, 0x15942a0),  # class vtable
        ("rip", 0x7c93c6, 7, 3, 0x325e200),  # bss object/cell
        ("rip", 0x7c93cd, 7, 3, 0x1287fee),  # rodata string
        ("rip", 0x7c93e1, 7, 3, 0x15ce318),  # functor vtable tag
        ("imm", 0x7c93fa, "41b901000000"),
        ("rip", 0x7c941a, 7, 3, 0x128800e),  # rodata string
        ("imm", 0x7c9421, "48c74424281a000000"),
        ("imm", 0x7c942a, "48c784249000000000000000"),
        ("imm", 0x7c944a, "c744241c00000000"),
        ("imm", 0x7c9452, "c744241800002041"),
        ("imm", 0x7c945d, "48c78424a800000000000000"),
        ("imm", 0x7c9480, "48c744245800000000"),
        ("call", 0x7c9491, 0xc300),
        ("call", 0x7c949d, 0xf590),
        ("imm", 0x7c94ad, "ff5018"),
        ("imm", 0x7c94b6, "c783d00000009a99193e"),
        ("imm", 0x7c94c0, "c743689a99193e"),
        ("call", 0x7c94c7, 0x20d60),
        ("call", 0x7c94cf, 0xf590),
        ("rip", 0x7c94d4, 7, 3, 0x20f0),  # destructor/function RVA
        ("rip", 0x7c94db, 7, 3, 0x1604218),  # dso handle
        ("call", 0x7c94e5, 0xf8f390),
        ("rip", 0x7c94ea, 7, 3, 0x15fe518),  # stack canary cell
        ("call", 0x7c950d, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_347_7c93a0.hpp', 'source': 'native/core/ctor_slot_347_7c93a0.cpp', 'test': 'native/tests/test_ctor_slot_347.cpp', 'shapes_h': 'native/core/ctor_shapes_343_356.hpp', 'shapes_c': 'native/core/ctor_shapes_343_356.cpp', 'cmake': 'native/CMakeLists.txt', 'consts': {'kSlot347Rva': 8164256, 'kSlot347Size': 370}, 'header_tokens': ['0x41200000u', '0x3e19999au', 'kFloatArgFnVtbl_347', 'true,'], 'source_order': ['init_float_var_static(objects.var, kSlot347Spec, concmd)'], 'test_tokens': ['kSlot347Rva == 0x7c93a0', 'section_alone()', 'section_failures()'], 'shapes_tokens': ['init_bounded_float_var_c300(var, name, &var.value, lo, hi, spec.flags, args, description)'], 'cmake_tokens': ['core/ctor_slot_347_7c93a0.cpp', 'add_executable(test_ctor_slot_347 tests/test_ctor_slot_347.cpp)', 'add_test(NAME ctor_slot_347 COMMAND test_ctor_slot_347)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Bounded float variable `nico_AltDiveAttackHoldTime` through 0xc300 (min 0, max 10.0, default 0x3e19999a, functor 0x15ce318).'))
