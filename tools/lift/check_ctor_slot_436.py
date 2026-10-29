#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 436 (RVA 0x50dbe0, 410 bytes), lane m3.

Bool console variable `shadow_showDOFStats` (object 0x2efc750, value 0, default 0, functor tag 0x15c6498) and a debug-menu toggle built by 0x26db0
(item 0x2efc818) under the parent path `Depth of Field`.

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
    "slot": 436,
    "base": 0x50dbe0,
    "size": 410,
    "body_sha256": "480304bc96ec269e277dfd447942dc68fa8033f1a3ee6d8d1008d11b96396c92",
    "pad": "909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_from_var_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15c6498],
    "float_tags": [],
    "strings": [
        (0x100f43c, b'shadow_showDOFStats'),
        (0x100f50d, b''),
        (0x100f450, b'Depth of Field'),
    ],
    "lengths": [
        ('shadow_showDOFStats', 0x50dc42, "48c7459813000000"),
        ('Depth of Field', 0x50dd30, "48c745b80e000000"),
    ],
    "pins": [
        ("rip", 0x50dbf1, 7, 3, 0x15fe518),
        ("rip", 0x50dbf8, 7, 3, 0x1594208),
        ("rip", 0x50dbff, 7, 3, 0x2efc750),
        ("rip", 0x50dc06, 7, 3, 0x100f43c),
        ("rip", 0x50dc0d, 7, 3, 0x100f50d),
        ("imm", 0x50dc20, "b901000000"),
        ("imm", 0x50dc42, "48c7459813000000"),
        ("rip", 0x50dc54, 7, 3, 0x2efc750),
        ("imm", 0x50dc5b, "48c745c800000000"),
        ("call", 0x50dc63, 0x20af0),
        ("rip", 0x50dc68, 7, 3, 0x15940a0),
        ("rip", 0x50dc72, 7, 3, 0x2efc810),
        ("rip", 0x50dc7d, 7, 3, 0x15c6498),
        ("rip", 0x50dc88, 7, 3, 0x2efc750),
        ("rip", 0x50dc8f, 7, 3, 0x2efc7b0),
        ("rip", 0x50dc96, 10, 2, 0x2efc7b9),
        ("imm", 0x50dc96, "c70519eb9e0200000000"),
        ("rip", 0x50dca0, 8, 4, 0x2efc7c0),
        ("imm", 0x50dca0, "c5fc110518eb9e02"),
        ("rip", 0x50dca8, 11, 3, 0x2efc7f0),
        ("imm", 0x50dca8, "48c7053deb9e0200000000"),
        ("rip", 0x50dcb3, 7, 3, 0x2efc800),
        ("rip", 0x50dcba, 11, 3, 0x2efc808),
        ("imm", 0x50dcba, "48c70543eb9e0200000000"),
        ("call", 0x50dcc5, 0xf590),
        ("rip", 0x50dccd, 7, 3, 0x2efc750),
        ("rip", 0x50dcd4, 7, 2, 0x2efc810),
        ("imm", 0x50dcd4, "c60535eb9e0200"),
        ("rip", 0x50dcdb, 7, 2, 0x2efc7b8),
        ("imm", 0x50dcdb, "c605d6ea9e0200"),
        ("call", 0x50dce2, 0x20d60),
        ("call", 0x50dcea, 0xf590),
        ("rip", 0x50dcef, 7, 3, 0x1604218),
        ("rip", 0x50dcf6, 7, 3, 0x20b0),
        ("call", 0x50dd03, 0xf8f390),
        ("rip", 0x50dd08, 7, 3, 0x2efc818),
        ("rip", 0x50dd13, 7, 3, 0x100f450),
        ("imm", 0x50dd1e, "b930000000"),
        ("imm", 0x50dd30, "48c745b80e000000"),
        ("call", 0x50dd3d, 0x26db0),
        ("call", 0x50dd45, 0xf590),
        ("rip", 0x50dd4a, 7, 3, 0x272d0),
        ("call", 0x50dd57, 0xf8f390),
        ("call", 0x50dd75, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_436_50dbe0.hpp', 'source': 'native/core/ctor_slot_436_50dbe0.cpp', 'test': 'native/tests/test_ctor_slot_436.cpp', 'cmake': 'native/lanes/m3.cmake', 'consts': {'kSlot436Rva': 5299168, 'kSlot436Size': 410}, 'header_tokens': ['kSlot436FnTag = 0x15c6498', 'kSlot436ObjectRva = 0x2efc750', 'kSlot436ItemRva = 0x2efc818'], 'source_order': ['init_bool_var_site(objects.var, kSlot436VarName, kSlot436FnTag, concmd, 0, 0)', 'init_toggle_site_26db0(objects.item, objects.var, kSlot436Parent, concmd, menu)'], 'test_tokens': ['kSlot436Rva == 0x50dbe0', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_436_50dbe0.cpp', 'add_executable(test_ctor_slot_436 tests/test_ctor_slot_436.cpp)', 'add_test(NAME ctor_slot_436 COMMAND test_ctor_slot_436)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'bool variable `shadow_showDOFStats` (value 0, tag 0x15c6498) + 0x26db0 toggle under `Depth of Field` (flags 0x30, f590(parent), atexit 0x272d0).'))
