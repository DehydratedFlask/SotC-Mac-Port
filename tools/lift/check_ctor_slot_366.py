#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 366 (RVA 0x744ac0, 695 bytes), batch 6 debug page.

Two bool console variables `shadow_smoothAvoidControlRootY` (value 1, object 0x3252120) and `shadow_debugAvoidControlRootY` (value 0, object 0x3252290), functor tag
0x15cd200, each followed by a 0x26db0 debug-menu toggle under the parent path `Shadow IK` (items 0x32521e8, 0x3252358).

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
    "slot": 366,
    "base": 0x744ac0,
    "size": 695,
    "body_sha256": "2aec5eb2ce229433641565f2b6edee8e471b8c833a4abe8be069c67b1856460c",
    "pad": "909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_from_var_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15cd200],
    "float_tags": [],
    "strings": [
        (0x127f3e0, b'shadow_smoothAvoidControlRootY'),
        (0x127f3ff, b'Shadow IK'),
        (0x127f409, b'shadow_debugAvoidControlRootY'),
        (0x127f450, b''),
    ],
    "lengths": [
        ('shadow_smoothAvoidControlRootY', 0x744b21, "48c745981e000000"),
        ('Shadow IK', 0x744bf1, "48c745b809000000"),
        ('shadow_debugAvoidControlRootY', 0x744c54, "48c745981d000000"),
        ('Shadow IK', 0x744d23, "48c745b809000000"),
    ],
    "pins": [
        ("rip", 0x744ad1, 7, 3, 0x15fe518),
        ("rip", 0x744ad8, 7, 3, 0x1594208),
        ("rip", 0x744adf, 7, 3, 0x3252120),
        ("rip", 0x744ae6, 7, 3, 0x127f3e0),
        ("rip", 0x744aed, 7, 3, 0x127f450),
        ("imm", 0x744b00, "b901000000"),
        ("imm", 0x744b08, "4983c410"),
        ("imm", 0x744b21, "48c745981e000000"),
        ("imm", 0x744b36, "48c745c800000000"),
        ("call", 0x744b3e, 0x20af0),
        ("rip", 0x744b43, 7, 3, 0x15940a0),
        ("rip", 0x744b58, 7, 3, 0x15cd200),
        ("imm", 0x744b5f, "4983c610"),
        ("imm", 0x744b6a, "c7436900000000"),
        ("imm", 0x744b76, "48c783a000000000000000"),
        ("imm", 0x744b88, "48c783b800000000000000"),
        ("call", 0x744b93, 0xf590),
        ("imm", 0x744b9e, "c683c000000001"),
        ("imm", 0x744ba5, "c6436801"),
        ("call", 0x744ba9, 0x20d60),
        ("call", 0x744bb1, 0xf590),
        ("rip", 0x744bb6, 7, 3, 0x20b0),
        ("rip", 0x744bbd, 7, 3, 0x1604218),
        ("call", 0x744bc7, 0xf8f390),
        ("rip", 0x744bcc, 7, 3, 0x32521e8),
        ("rip", 0x744bd7, 7, 3, 0x127f3ff),
        ("imm", 0x744be2, "b930000000"),
        ("imm", 0x744bf1, "48c745b809000000"),
        ("call", 0x744c01, 0x26db0),
        ("call", 0x744c09, 0xf590),
        ("rip", 0x744c0e, 7, 3, 0x272d0),
        ("rip", 0x744c15, 7, 3, 0x1604218),
        ("call", 0x744c1f, 0xf8f390),
        ("rip", 0x744c24, 7, 3, 0x3252290),
        ("rip", 0x744c2b, 7, 3, 0x127f409),
        ("imm", 0x744c3e, "b901000000"),
        ("rip", 0x744c47, 7, 3, 0x127f450),
        ("imm", 0x744c54, "48c745981d000000"),
        ("imm", 0x744c6a, "48c745c800000000"),
        ("call", 0x744c78, 0x20af0),
        ("rip", 0x744c8b, 7, 3, 0x15cd200),
        ("imm", 0x744c99, "c7436900000000"),
        ("imm", 0x744ca5, "48c783a000000000000000"),
        ("imm", 0x744cb7, "48c783b800000000000000"),
        ("call", 0x744cc2, 0xf590),
        ("imm", 0x744ccd, "c683c000000000"),
        ("imm", 0x744cd4, "c6436800"),
        ("call", 0x744cd8, 0x20d60),
        ("call", 0x744ce0, 0xf590),
        ("rip", 0x744ce5, 7, 3, 0x1604218),
        ("rip", 0x744cec, 7, 3, 0x20b0),
        ("call", 0x744cf9, 0xf8f390),
        ("rip", 0x744cfe, 7, 3, 0x3252358),
        ("rip", 0x744d09, 7, 3, 0x127f3ff),
        ("imm", 0x744d14, "b930000000"),
        ("imm", 0x744d23, "48c745b809000000"),
        ("call", 0x744d33, 0x26db0),
        ("call", 0x744d3b, 0xf590),
        ("rip", 0x744d40, 7, 3, 0x272d0),
        ("call", 0x744d4d, 0xf8f390),
        ("rip", 0x744d52, 7, 3, 0x15fe518),
        ("call", 0x744d71, 0xf8f3a0),
    ],
    "m6_extras": ['bp_atexit_imports', 'bp_object_vtables'],
    "native": {'header': 'native/core/ctor_slot_366_744ac0.hpp', 'source': 'native/core/ctor_slot_366_744ac0.cpp', 'test': 'native/tests/test_ctor_slot_366.cpp', 'consts': {'kSlot366Rva': 7621312, 'kSlot366Size': 695, 'kSlot366FnTag': 22860288, 'kSlot366Var0NameLength': 30, 'kSlot366Var1NameLength': 29, 'kSlot366ParentLength': 9},
        'header_tokens': ['kSlot366Var0Rva = 0x3252120', 'kSlot366Item0Rva = 0x32521e8', 'kSlot366Var1Rva = 0x3252290', 'kSlot366Item1Rva = 0x3252358', 'kSlot366Var0Name[] = "shadow_smoothAvoidControlRootY"', 'kSlot366Var1Name[] = "shadow_debugAvoidControlRootY"', 'kSlot366Parent[] = "Shadow IK"', 'IDENTITY TAGS'], 'source_order': ['lane_m6::init_bool_var_literal(objects.var0, kSlot366Var0Name, kSlot366Var0NameLength, "", 0, kSlot366FnTag,', 'init_toggle_site_26db0(objects.item0, objects.var0, kSlot366Parent, concmd, menu)', 'lane_m6::init_bool_var_literal(objects.var1, kSlot366Var1Name, kSlot366Var1NameLength, "", 0, kSlot366FnTag,', 'init_toggle_site_26db0(objects.item1, objects.var1, kSlot366Parent, concmd, menu)'], 'test_tokens': ['kSlot366Rva == 0x744ac0 && kSlot366Size == 695', 'section_alone()', 'section_failures()', 'CHECK(creg.atexit_log.size() == 5);', 'CHECK(!r.ok']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, '2 bool variables (tag 0x15cd200, values 1/0) + two 0x26db0 toggles under `Shadow IK`'))
