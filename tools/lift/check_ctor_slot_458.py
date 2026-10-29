#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 458 (RVA 0x45ace0, 323 bytes), lane m3.

One bool console variable `showFootDrivenMotion` (object 0x2ef7ff8, value 0, default 0, functor tag 0x15bab98). Same stores as slot 348, rip-relative encoding.

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
    "slot": 458,
    "base": 0x45ace0,
    "size": 323,
    "body_sha256": "3ac884610a3b407ae67b4e92576e2c6817a4068613f33613b3320b724b915b2f",
    "pad": "90909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15bab98],
    "float_tags": [],
    "strings": [
        (0xff6d14, b'showFootDrivenMotion'),
        (0xff6d5d, b''),
    ],
    "lengths": [
        ('showFootDrivenMotion', 0x45ad42, "48c7459814000000"),
    ],
    "pins": [
        ("rip", 0x45acf1, 7, 3, 0x15fe518),
        ("rip", 0x45acf8, 7, 3, 0x1594208),
        ("rip", 0x45acff, 7, 3, 0x2ef7ff8),
        ("rip", 0x45ad06, 7, 3, 0xff6d14),
        ("rip", 0x45ad0d, 7, 3, 0xff6d5d),
        ("imm", 0x45ad20, "b901000000"),
        ("imm", 0x45ad42, "48c7459814000000"),
        ("rip", 0x45ad54, 7, 3, 0x2ef7ff8),
        ("imm", 0x45ad5b, "48c745c800000000"),
        ("call", 0x45ad63, 0x20af0),
        ("rip", 0x45ad68, 7, 3, 0x15940a0),
        ("rip", 0x45ad72, 7, 3, 0x2ef80b8),
        ("rip", 0x45ad7d, 7, 3, 0x15bab98),
        ("rip", 0x45ad88, 7, 3, 0x2ef7ff8),
        ("rip", 0x45ad8f, 7, 3, 0x2ef8058),
        ("rip", 0x45ad96, 10, 2, 0x2ef8061),
        ("imm", 0x45ad96, "c705c1d2a90200000000"),
        ("rip", 0x45ada0, 8, 4, 0x2ef8068),
        ("imm", 0x45ada0, "c5fc1105c0d2a902"),
        ("rip", 0x45ada8, 11, 3, 0x2ef8098),
        ("imm", 0x45ada8, "48c705e5d2a90200000000"),
        ("rip", 0x45adb3, 7, 3, 0x2ef80a8),
        ("rip", 0x45adba, 11, 3, 0x2ef80b0),
        ("imm", 0x45adba, "48c705ebd2a90200000000"),
        ("call", 0x45adc5, 0xf590),
        ("rip", 0x45adcd, 7, 3, 0x2ef7ff8),
        ("rip", 0x45add4, 7, 2, 0x2ef80b8),
        ("imm", 0x45add4, "c605ddd2a90200"),
        ("rip", 0x45addb, 7, 2, 0x2ef8060),
        ("imm", 0x45addb, "c6057ed2a90200"),
        ("call", 0x45ade2, 0x20d60),
        ("call", 0x45adea, 0xf590),
        ("rip", 0x45adef, 7, 3, 0x20b0),
        ("rip", 0x45adf6, 7, 3, 0x1604218),
        ("call", 0x45ae00, 0xf8f390),
        ("call", 0x45ae1e, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_458_45ace0.hpp', 'source': 'native/core/ctor_slot_458_45ace0.cpp', 'test': 'native/tests/test_ctor_slot_458.cpp', 'cmake': 'native/lanes/m3.cmake', 'consts': {'kSlot458Rva': 4566240, 'kSlot458Size': 323}, 'header_tokens': ['kSlot458FnTag = 0x15bab98', 'kSlot458ObjectRva = 0x2ef7ff8'], 'source_order': ['init_bool_var_site(objects.var, kSlot458Name, kSlot458FnTag, concmd, 0, 0)'], 'test_tokens': ['kSlot458Rva == 0x45ace0', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_458_45ace0.cpp', 'add_executable(test_ctor_slot_458 tests/test_ctor_slot_458.cpp)', 'add_test(NAME ctor_slot_458 COMMAND test_ctor_slot_458)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'One bool console variable `showFootDrivenMotion` (value 0, default 0, functor tag 0x15bab98, flags 1): 0x20af0, f590(desc), 0x20d60, f590(name), atexit(0x20b0).'))
