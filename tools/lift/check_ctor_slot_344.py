#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 344 (RVA 0x7d3810, 706 bytes), lane L4.

Two flag/atomic cells, bool variable `ShowCinematicTime` (tag 0x15ce3b0), a 0x26db0 menu toggle under `Cutscene`, and the command `cutscene_nextCut` (handler tag 0x7d2c20).

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
    "slot": 344,
    "base": 0x7d3810,
    "size": 706,
    "body_sha256": "1a6c38756dcfe33f3ebdbd008c21eb4f7e8ce2b8831aa00782df184d1cfc4e23",
    "pad": "9090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'cmd_dtor', 'menu_toggle_from_var_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15ce3b0],
    "float_tags": [],
    "strings": [
        (0x1288a50, b'ShowCinematicTime'),
        (0x1288b91, b''),
        (0x1288a62, b'Cutscene'),
        (0x1288a6b, b'cutscene_nextCut'),
        (0x1288a7c, b'Skip to the next cut in the current cutscene'),
    ],
    "lengths": [
        ('Skip to the next cut in the current cutscene', 0x7d3a0e, "48c74424282c000000"),
    ],
    "pins": [
        ("rip", 0x7d3828, 7, 3, 0x15fe518),  # stack canary cell
        ("imm", 0x7d382f, "48ba010000000000803f"),
        ("rip", 0x7d383b, 7, 3, 0x1594208),  # class vtable
        ("rip", 0x7d3842, 7, 3, 0x325e910),  # bss object/cell
        ("rip", 0x7d386a, 7, 3, 0x325e8d0),  # bss object/cell
        ("rip", 0x7d3871, 7, 3, 0x1288a50),  # rodata string
        ("rip", 0x7d3878, 6, 2, 0x325e8e0),  # bss object/cell
        ("rip", 0x7d387e, 7, 3, 0x325e8f0),  # bss object/cell
        ("rip", 0x7d388a, 6, 2, 0x325e900),  # bss object/cell
        ("rip", 0x7d3895, 7, 3, 0x1288b91),  # rodata string
        ("imm", 0x7d389c, "b901000000"),
        ("imm", 0x7d38a1, "48c744242811000000"),
        ("rip", 0x7d38b0, 7, 3, 0x325e910),  # bss object/cell
        ("imm", 0x7d38c2, "48c744245800000000"),
        ("call", 0x7d38cb, 0x20af0),
        ("rip", 0x7d38d0, 7, 3, 0x15940a0),  # class vtable
        ("rip", 0x7d38da, 7, 3, 0x325e9d0),  # bss object/cell
        ("rip", 0x7d38e5, 7, 3, 0x15ce3b0),  # functor vtable tag
        ("rip", 0x7d38f0, 7, 3, 0x325e910),  # bss object/cell
        ("rip", 0x7d38f7, 7, 3, 0x325e970),  # bss object/cell
        ("rip", 0x7d38fe, 10, 2, 0x325e979),  # bss object/cell
        ("rip", 0x7d3908, 8, 4, 0x325e980),  # bss object/cell
        ("rip", 0x7d3910, 11, 3, 0x325e9b0),  # bss object/cell
        ("rip", 0x7d391b, 7, 3, 0x325e9c0),  # bss object/cell
        ("rip", 0x7d3922, 11, 3, 0x325e9c8),  # bss object/cell
        ("call", 0x7d392d, 0xf590),
        ("rip", 0x7d3935, 7, 3, 0x325e910),  # bss object/cell
        ("rip", 0x7d393c, 7, 2, 0x325e9d0),  # bss object/cell
        ("rip", 0x7d3943, 7, 2, 0x325e978),  # bss object/cell
        ("call", 0x7d394a, 0x20d60),
        ("call", 0x7d3952, 0xf590),
        ("rip", 0x7d3957, 7, 3, 0x1604218),  # dso handle
        ("rip", 0x7d395e, 7, 3, 0x20b0),  # destructor/function RVA
        ("call", 0x7d396b, 0xf8f390),
        ("rip", 0x7d3970, 7, 3, 0x325e9d8),  # bss object/cell
        ("rip", 0x7d397c, 7, 3, 0x1288a62),  # rodata string
        ("imm", 0x7d3987, "b930000000"),
        ("imm", 0x7d399a, "48c744244808000000"),
        ("call", 0x7d39a9, 0x26db0),
        ("call", 0x7d39b1, 0xf590),
        ("rip", 0x7d39b6, 7, 3, 0x272d0),  # destructor/function RVA
        ("call", 0x7d39c3, 0xf8f390),
        ("rip", 0x7d39cd, 7, 3, 0x1288a6b),  # rodata string
        ("call", 0x7d39d9, 0xf2f0),
        ("rip", 0x7d39de, 7, 3, 0x325ea80),  # bss object/cell
        ("rip", 0x7d39e5, 8, 4, 0x15fe5e0),  #
        ("rip", 0x7d39ed, 7, 3, 0x1288a7c),  # rodata string
        ("imm", 0x7d39fd, "b902000000"),
        ("imm", 0x7d3a0e, "48c74424282c000000"),
        ("rip", 0x7d3a21, 7, 3, 0x7d2c20),  #
        ("call", 0x7d3a36, 0x20af0),
        ("rip", 0x7d3a3b, 7, 3, 0x1594dc0),  # class vtable
        ("imm", 0x7d3a7b, "48c7839000000000000000"),
        ("imm", 0x7d3a86, "ff5020"),
        ("call", 0x7d3a8e, 0xf590),
        ("call", 0x7d3a98, 0xf590),
        ("rip", 0x7d3a9d, 7, 3, 0x4b0),  # destructor/function RVA
        ("rip", 0x7d3aa4, 7, 3, 0x1604218),  # dso handle
        ("call", 0x7d3aae, 0xf8f390),
        ("call", 0x7d3acd, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_344_7d3810.hpp', 'source': 'native/core/ctor_slot_344_7d3810.cpp', 'test': 'native/tests/test_ctor_slot_344.cpp', 'shapes_h': 'native/core/ctor_shapes_343_356.hpp', 'shapes_c': 'native/core/ctor_shapes_343_356.cpp', 'cmake': 'native/CMakeLists.txt', 'consts': {'kSlot344Rva': 8206352, 'kSlot344Size': 706}, 'header_tokens': ['kSlot344FnTag = 0x15ce3b0', 'kSlot344HandlerTag = 0x7d2c20', 'kSlot344CommandDescriptionLength = 0x2c'], 'source_order': ['init_flag_scale_cell(objects.cell_a)', 'init_flag_scale_cell(objects.cell_b)', 'init_bool_var_site(objects.var, kSlot344VarName, kSlot344FnTag, concmd, 0, 0)', 'init_toggle_site_26db0(objects.item, objects.var, kSlot344Parent, concmd, menu)', 'init_fconcmd_object(objects.command, spec, concmd)'], 'test_tokens': ['kSlot344Rva == 0x7d3810', 'section_alone()', 'section_failures()'], 'shapes_tokens': ['init_menu_toggle_from_var_26db0(item, p, target, kL4MenuItemFlags)', 'std::atomic_ref<std::int32_t>(cell.atomic_word).store(0, std::memory_order_seq_cst)', 'cell.flag_scale = kFlagScaleQword'], 'cmake_tokens': ['core/ctor_slot_344_7d3810.cpp', 'add_executable(test_ctor_slot_344 tests/test_ctor_slot_344.cpp)', 'add_test(NAME ctor_slot_344 COMMAND test_ctor_slot_344)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Two flag/atomic cells, bool variable `ShowCinematicTime` (tag 0x15ce3b0), a 0x26db0 menu toggle under `Cutscene`, and the command `cutscene_nextCut` (handler tag 0x7d2c20).'))
