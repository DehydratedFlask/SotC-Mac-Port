#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 355 (RVA 0x7946d0, 646 bytes), lane L4.

Two bool variables (`shadow_yamoriBossDead`, `shadow_yamoriBossInfiniteStun`; tag 0x15ce050, 0/0) and a 0x26ea0 toggle that targets the second one.

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
    "slot": 355,
    "base": 0x7946d0,
    "size": 646,
    "body_sha256": "7ac88091c13400eb1942ec998208422f8bf1bb5aa783fd6c02cc598395d5a703",
    "pad": "9090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15ce050],
    "float_tags": [],
    "strings": [
        (0x1284ae0, b'shadow_yamoriBossDead'),
        (0x1284c3b, b''),
        (0x1284af6, b'shadow_yamoriBossInfiniteStun'),
        (0x1284b14, b'Shadow AI'),
        (0x1284b1e, b'Yamori Boss Infinite Stun'),
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x7946e1, 7, 3, 0x15fe518),  # stack canary cell
        ("rip", 0x7946e8, 7, 3, 0x1594208),  # class vtable
        ("rip", 0x7946ef, 7, 3, 0x3259270),  # bss object/cell
        ("rip", 0x7946f6, 7, 3, 0x1284ae0),  # rodata string
        ("rip", 0x7946fd, 7, 3, 0x1284c3b),  # rodata string
        ("imm", 0x794710, "b901000000"),
        ("imm", 0x794731, "48c7459815000000"),
        ("imm", 0x794746, "48c745c800000000"),
        ("call", 0x79474e, 0x20af0),
        ("rip", 0x794753, 7, 3, 0x15940a0),  # class vtable
        ("rip", 0x794768, 7, 3, 0x15ce050),  # functor vtable tag
        ("imm", 0x79477a, "c7436900000000"),
        ("imm", 0x794786, "48c783a000000000000000"),
        ("imm", 0x794798, "48c783b800000000000000"),
        ("call", 0x7947a3, 0xf590),
        ("imm", 0x7947ae, "c683c000000000"),
        ("imm", 0x7947b5, "c6436800"),
        ("call", 0x7947b9, 0x20d60),
        ("call", 0x7947c1, 0xf590),
        ("rip", 0x7947c6, 7, 3, 0x20b0),  # destructor/function RVA
        ("rip", 0x7947cd, 7, 3, 0x1604218),  # dso handle
        ("call", 0x7947d7, 0xf8f390),
        ("rip", 0x7947dc, 7, 3, 0x3259338),  # bss object/cell
        ("rip", 0x7947e3, 7, 3, 0x1284af6),  # rodata string
        ("imm", 0x7947f6, "b901000000"),
        ("rip", 0x7947ff, 7, 3, 0x1284c3b),  # rodata string
        ("imm", 0x79480c, "48c745981d000000"),
        ("imm", 0x794822, "48c745c800000000"),
        ("call", 0x794830, 0x20af0),
        ("rip", 0x794843, 7, 3, 0x15ce050),  # functor vtable tag
        ("imm", 0x794851, "c7436900000000"),
        ("imm", 0x79485d, "48c783a000000000000000"),
        ("imm", 0x79486f, "48c783b800000000000000"),
        ("call", 0x79487a, 0xf590),
        ("imm", 0x794885, "c683c000000000"),
        ("imm", 0x79488c, "c6436800"),
        ("call", 0x794890, 0x20d60),
        ("call", 0x794898, 0xf590),
        ("rip", 0x79489d, 7, 3, 0x1604218),  # dso handle
        ("rip", 0x7948a4, 7, 3, 0x20b0),  # destructor/function RVA
        ("call", 0x7948b1, 0xf8f390),
        ("rip", 0x7948b6, 7, 3, 0x3259400),  # bss object/cell
        ("rip", 0x7948bd, 7, 3, 0x1284b14),  # rodata string
        ("imm", 0x7948d0, "41b830000000"),
        ("rip", 0x7948dd, 7, 3, 0x1284b1e),  # rodata string
        ("imm", 0x7948ea, "48c745b809000000"),
        ("imm", 0x7948fb, "48c7459819000000"),
        ("call", 0x79490b, 0x26ea0),
        ("call", 0x794913, 0xf590),
        ("call", 0x79491b, 0xf590),
        ("rip", 0x794920, 7, 3, 0x272d0),  # destructor/function RVA
        ("call", 0x79492d, 0xf8f390),
        ("rip", 0x794932, 7, 3, 0x15fe518),  # stack canary cell
        ("call", 0x794951, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_355_7946d0.hpp', 'source': 'native/core/ctor_slot_355_7946d0.cpp', 'test': 'native/tests/test_ctor_slot_355.cpp', 'shapes_h': 'native/core/ctor_shapes_343_356.hpp', 'shapes_c': 'native/core/ctor_shapes_343_356.cpp', 'cmake': 'native/CMakeLists.txt', 'consts': {'kSlot355Rva': 7947984, 'kSlot355Size': 646}, 'header_tokens': ['kSlot355FnTag = 0x15ce050'], 'source_order': ['init_bool_var_site(objects.var_a, kSlot355NameA', 'init_bool_var_site(objects.var_b, kSlot355NameB', 'init_toggle_site_26ea0(objects.item, objects.var_b, kSlot355Label, kSlot355Text, concmd, menu)'], 'test_tokens': ['kSlot355Rva == 0x7946d0', 'section_alone()', 'section_failures()'], 'shapes_tokens': ['release_inert_view_f590(t) && release_inert_view_f590(l)'], 'cmake_tokens': ['core/ctor_slot_355_7946d0.cpp', 'add_executable(test_ctor_slot_355 tests/test_ctor_slot_355.cpp)', 'add_test(NAME ctor_slot_355 COMMAND test_ctor_slot_355)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Two bool variables (`shadow_yamoriBossDead`, `shadow_yamoriBossInfiniteStun`; tag 0x15ce050, 0/0) and a 0x26ea0 toggle that targets the second one.'))
