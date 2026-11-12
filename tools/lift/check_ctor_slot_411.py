#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 411 (RVA 0x591710, 692 bytes), batch 6 debug page.

Two bool console variables `shadow_cameraUsePureLwsOrientation` (value 1, object 0x2fd98a0) and `shadow_cameraDebugPureLwsOrientation` (value 0, object 0x2fd9968), functor tag
0x15cb168, each followed by a 0x26db0 debug-menu toggle under the parent path `Shadow Camera` (items 0x2fd9a30, 0x2fd9ad8).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA.
Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call target, immediate and
indirect call of the body (generated from the disassembly, re-verified here from bytes), the referenced strings, the
bool functor vtable of the caller tag and the hash of every shared callee. The inherited M6 driver runs a fail-closed
mutation self-test (first/last byte of every pin, relocation perturbation, native-token perturbation); --tamper
[elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 411,
    "base": 0x591710,
    "size": 692,
    "body_sha256": "b7deef0503476aea502554adb743740fbf5203f363fa46001572ac9df9836ebe",
    "pad": "909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_from_var_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15cb168],
    "float_tags": [],
    "strings": [
        (0x1263f40, b'shadow_cameraUsePureLwsOrientation'),
        (0x1263f63, b'shadow_cameraDebugPureLwsOrientation'),
        (0x1263f88, b'Shadow Camera'),
        (0x1263fa1, b''),
    ],
    "lengths": [
        ('shadow_cameraUsePureLwsOrientation', 0x591771, "48c7459822000000"),
        ('shadow_cameraDebugPureLwsOrientation', 0x59184d, "48c7459824000000"),
        ('Shadow Camera', 0x59191d, "48c745b80d000000"),
        ('Shadow Camera', 0x591974, "48c745b80d000000"),
    ],
    "pins": [
        ("rip", 0x591721, 7, 3, 0x15fe518),
        ("rip", 0x591728, 7, 3, 0x1594208),
        ("rip", 0x59172f, 7, 3, 0x2fd98a0),
        ("rip", 0x591736, 7, 3, 0x1263f40),
        ("rip", 0x59173d, 7, 3, 0x1263fa1),
        ("imm", 0x591750, "b901000000"),
        ("imm", 0x591758, "4983c610"),
        ("imm", 0x591771, "48c7459822000000"),
        ("imm", 0x591786, "48c745c800000000"),
        ("call", 0x59178e, 0x20af0),
        ("rip", 0x591793, 7, 3, 0x15940a0),
        ("rip", 0x5917a8, 7, 3, 0x15cb168),
        ("imm", 0x5917af, "4983c510"),
        ("imm", 0x5917ba, "41c7476900000000"),
        ("imm", 0x5917c8, "49c787a000000000000000"),
        ("imm", 0x5917da, "49c787b800000000000000"),
        ("call", 0x5917e5, 0xf590),
        ("imm", 0x5917f0, "41c687c000000001"),
        ("imm", 0x5917f8, "41c6476801"),
        ("call", 0x5917fd, 0x20d60),
        ("call", 0x591805, 0xf590),
        ("rip", 0x59180a, 7, 3, 0x20b0),
        ("rip", 0x591811, 7, 3, 0x1604218),
        ("call", 0x59181b, 0xf8f390),
        ("rip", 0x591820, 7, 3, 0x2fd9968),
        ("rip", 0x591827, 7, 3, 0x1263f63),
        ("imm", 0x59183a, "b901000000"),
        ("rip", 0x591843, 7, 3, 0x1263fa1),
        ("imm", 0x59184d, "48c7459824000000"),
        ("imm", 0x591863, "48c745c800000000"),
        ("call", 0x591871, 0x20af0),
        ("rip", 0x591884, 7, 3, 0x15cb168),
        ("imm", 0x591892, "c7436900000000"),
        ("imm", 0x59189e, "48c783a000000000000000"),
        ("imm", 0x5918b0, "48c783b800000000000000"),
        ("call", 0x5918bb, 0xf590),
        ("imm", 0x5918c6, "c683c000000000"),
        ("imm", 0x5918cd, "c6436800"),
        ("call", 0x5918d1, 0x20d60),
        ("call", 0x5918da, 0xf590),
        ("rip", 0x5918df, 7, 3, 0x1604218),
        ("rip", 0x5918e6, 7, 3, 0x20b0),
        ("call", 0x5918f3, 0xf8f390),
        ("rip", 0x5918f8, 7, 3, 0x2fd9a30),
        ("rip", 0x591903, 7, 3, 0x1263f88),
        ("imm", 0x59190e, "b930000000"),
        ("imm", 0x59191d, "48c745b80d000000"),
        ("call", 0x59192d, 0x26db0),
        ("call", 0x591935, 0xf590),
        ("rip", 0x59193a, 7, 3, 0x272d0),
        ("call", 0x59194a, 0xf8f390),
        ("rip", 0x59194f, 7, 3, 0x2fd9ad8),
        ("rip", 0x59195a, 7, 3, 0x1263f88),
        ("imm", 0x591965, "b930000000"),
        ("imm", 0x591974, "48c745b80d000000"),
        ("call", 0x591984, 0x26db0),
        ("call", 0x59198c, 0xf590),
        ("call", 0x59199a, 0xf8f390),
        ("rip", 0x59199f, 7, 3, 0x15fe518),
        ("call", 0x5919be, 0xf8f3a0),
    ],
    "m6_extras": ['bp_atexit_imports', 'bp_object_vtables'],
    "native": {'header': 'native/core/ctor_slot_411_591710.hpp', 'source': 'native/core/ctor_slot_411_591710.cpp', 'test': 'native/tests/test_ctor_slot_411.cpp', 'consts': {'kSlot411Rva': 5838608, 'kSlot411Size': 692, 'kSlot411FnTag': 22851944, 'kSlot411Var0NameLength': 34, 'kSlot411Var1NameLength': 36, 'kSlot411ParentLength': 13},
        'header_tokens': ['kSlot411Var0Rva = 0x2fd98a0', 'kSlot411Item0Rva = 0x2fd9a30', 'kSlot411Var1Rva = 0x2fd9968', 'kSlot411Item1Rva = 0x2fd9ad8', 'kSlot411Var0Name[] = "shadow_cameraUsePureLwsOrientation"', 'kSlot411Var1Name[] = "shadow_cameraDebugPureLwsOrientation"', 'kSlot411Parent[] = "Shadow Camera"', 'IDENTITY TAGS'], 'source_order': ['lane_m6::init_bool_var_literal(objects.var0, kSlot411Var0Name, kSlot411Var0NameLength, "", 0, kSlot411FnTag,', 'init_toggle_site_26db0(objects.item0, objects.var0, kSlot411Parent, concmd, menu)', 'lane_m6::init_bool_var_literal(objects.var1, kSlot411Var1Name, kSlot411Var1NameLength, "", 0, kSlot411FnTag,', 'init_toggle_site_26db0(objects.item1, objects.var1, kSlot411Parent, concmd, menu)'], 'test_tokens': ['kSlot411Rva == 0x591710 && kSlot411Size == 692', 'section_alone()', 'section_failures()', 'CHECK(creg.atexit_log.size() == 5);', 'CHECK(!r.ok']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, '2 bool variables (tag 0x15cb168, values 1/0) + two 0x26db0 toggles under `Shadow Camera`'))
