#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 455 (RVA 0x47bfa0, 817 bytes), batch 6 debug page.

Two bool console variables `looseAttachments_Enable` (value 1, object 0x2ef8250) and `looseAttachments_ShowCollision` (value 0, object 0x2ef8318), functor tag 0x15bddb8,
then a 0x26c10 debug-menu group `Loose Attachment` under the parent path `Animation` (flags 0x20, item 0x2ef83e0), then two 0x26db0 toggles for the variables under
the parent path `Loose Attachment` (flags 0x30, items 0x2ef8488, 0x2ef8530).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test;
--tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 455,
    "base": 0x47bfa0,
    "size": 817,
    "body_sha256": "3fe3a873c9cfe2e677f5fd551f7056b922224faed19cd2acd141372a9a52a34c",
    "pad": "909090909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_from_var_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15bddb8],
    "float_tags": [],
    "strings": [
        (0xffa720, b'looseAttachments_Enable'),
        (0xffa738, b'looseAttachments_ShowCollision'),
        (0xffa757, b'Animation'),
        (0xffa761, b'Loose Attachment'),
        (0xffa78d, b''),
    ],
    "lengths": [
        ('looseAttachments_Enable', 0x47c001, "48c7459817000000"),
        ('looseAttachments_ShowCollision', 0x47c0dd, "48c745981e000000"),
        ('Animation', 0x47c1bc, "48c745b809000000"),
        ('Loose Attachment', 0x47c1c4, "48c7459810000000"),
        ('Loose Attachment', 0x47c22c, "48c745b810000000"),
        ('Loose Attachment', 0x47c280, "48c745b810000000"),
        ('looseAttachments_Enable', 0x47c001, "48c7459817000000"),
        ('looseAttachments_ShowCollision', 0x47c0dd, "48c745981e000000"),
        ('Animation', 0x47c1bc, "48c745b809000000"),
        ('Loose Attachment', 0x47c1c4, "48c7459810000000"),
        ('Loose Attachment', 0x47c22c, "48c745b810000000"),
        ('Loose Attachment', 0x47c280, "48c745b810000000"),
    ],
    "pins": [
        ("rip", 0x47bfb1, 7, 3, 0x15fe518),
        ("rip", 0x47bfb8, 7, 3, 0x1594208),
        ("rip", 0x47bfbf, 7, 3, 0x2ef8250),
        ("rip", 0x47bfc6, 7, 3, 0xffa720),
        ("rip", 0x47bfcd, 7, 3, 0xffa78d),
        ("imm", 0x47bfe0, "b901000000"),
        ("imm", 0x47bfe8, "4983c410"),
        ("imm", 0x47c001, "48c7459817000000"),
        ("imm", 0x47c016, "48c745c800000000"),
        ("call", 0x47c01e, 0x20af0),
        ("rip", 0x47c023, 7, 3, 0x15940a0),
        ("rip", 0x47c038, 7, 3, 0x15bddb8),
        ("imm", 0x47c03f, "4983c510"),
        ("imm", 0x47c04a, "41c7476900000000"),
        ("imm", 0x47c058, "49c787a000000000000000"),
        ("imm", 0x47c06a, "49c787b800000000000000"),
        ("call", 0x47c075, 0xf590),
        ("imm", 0x47c080, "41c687c000000001"),
        ("imm", 0x47c088, "41c6476801"),
        ("call", 0x47c08d, 0x20d60),
        ("call", 0x47c095, 0xf590),
        ("rip", 0x47c09a, 7, 3, 0x20b0),
        ("rip", 0x47c0a1, 7, 3, 0x1604218),
        ("call", 0x47c0ab, 0xf8f390),
        ("rip", 0x47c0b0, 7, 3, 0x2ef8318),
        ("rip", 0x47c0b7, 7, 3, 0xffa738),
        ("imm", 0x47c0ca, "b901000000"),
        ("rip", 0x47c0d3, 7, 3, 0xffa78d),
        ("imm", 0x47c0dd, "48c745981e000000"),
        ("imm", 0x47c0f3, "48c745c800000000"),
        ("call", 0x47c101, 0x20af0),
        ("rip", 0x47c114, 7, 3, 0x15bddb8),
        ("imm", 0x47c122, "c7436900000000"),
        ("imm", 0x47c12e, "48c783a000000000000000"),
        ("imm", 0x47c140, "48c783b800000000000000"),
        ("call", 0x47c14b, 0xf590),
        ("imm", 0x47c156, "c683c000000000"),
        ("imm", 0x47c15d, "c6436800"),
        ("call", 0x47c161, 0x20d60),
        ("call", 0x47c16a, 0xf590),
        ("rip", 0x47c16f, 7, 3, 0x1604218),
        ("rip", 0x47c176, 7, 3, 0x20b0),
        ("call", 0x47c183, 0xf8f390),
        ("rip", 0x47c188, 7, 3, 0xffa761),
        ("rip", 0x47c197, 7, 3, 0xffa757),
        ("rip", 0x47c1a2, 7, 3, 0x2ef83e0),
        ("imm", 0x47c1a9, "b920000000"),
        ("imm", 0x47c1bc, "48c745b809000000"),
        ("imm", 0x47c1c4, "48c7459810000000"),
        ("call", 0x47c1d6, 0x26c10),
        ("call", 0x47c1de, 0xf590),
        ("call", 0x47c1e6, 0xf590),
        ("rip", 0x47c1eb, 7, 3, 0x272d0),
        ("rip", 0x47c1f2, 7, 3, 0x2ef83e0),
        ("call", 0x47c1ff, 0xf8f390),
        ("rip", 0x47c204, 7, 3, 0x2ef8488),
        ("rip", 0x47c20f, 7, 3, 0xffa761),
        ("imm", 0x47c21a, "b930000000"),
        ("imm", 0x47c22c, "48c745b810000000"),
        ("call", 0x47c239, 0x26db0),
        ("call", 0x47c241, 0xf590),
        ("rip", 0x47c246, 7, 3, 0x272d0),
        ("call", 0x47c253, 0xf8f390),
        ("rip", 0x47c258, 7, 3, 0x2ef8530),
        ("rip", 0x47c263, 7, 3, 0xffa761),
        ("imm", 0x47c26e, "b930000000"),
        ("imm", 0x47c280, "48c745b810000000"),
        ("call", 0x47c28d, 0x26db0),
        ("call", 0x47c295, 0xf590),
        ("rip", 0x47c29a, 7, 3, 0x272d0),
        ("call", 0x47c2a7, 0xf8f390),
        ("rip", 0x47c2ac, 7, 3, 0x15fe518),
        ("call", 0x47c2cb, 0xf8f3a0),
    ],
    "m6_extras": ['bp_atexit_imports', 'bp_object_vtables'],
    "native": {'header': 'native/core/ctor_slot_455_47bfa0.hpp', 'source': 'native/core/ctor_slot_455_47bfa0.cpp', 'test': 'native/tests/test_ctor_slot_455.cpp', 'consts': {'kSlot455Rva': 4702112, 'kSlot455Size': 817, 'kSlot455FnTag': 22797752, 'kSlot455Var0NameLength': 23, 'kSlot455Var1NameLength': 30, 'kSlot455GroupParentLength': 9, 'kSlot455GroupCaptionLength': 16, 'kSlot455GroupFlags': 32},
        'header_tokens': ['kSlot455Var0Rva = 0x2ef8250', 'kSlot455Var1Rva = 0x2ef8318', 'kSlot455GroupRva = 0x2ef83e0', 'kSlot455Item0Rva = 0x2ef8488', 'kSlot455Item1Rva = 0x2ef8530', 'kSlot455Var0Name[] = "looseAttachments_Enable"', 'kSlot455Var1Name[] = "looseAttachments_ShowCollision"', 'kSlot455GroupParent[] = "Animation"', 'kSlot455GroupCaption[] = "Loose Attachment"', 'IDENTITY TAGS'], 'source_order': ['lane_m6::init_bool_var_literal(objects.var0, kSlot455Var0Name, kSlot455Var0NameLength, "", 0, kSlot455FnTag,', 'lane_m6::init_bool_var_literal(objects.var1, kSlot455Var1Name, kSlot455Var1NameLength, "", 0, kSlot455FnTag,', 'family_group_item_26c10(objects.group, kSlot455GroupParent, kSlot455GroupParentLength, kSlot455GroupCaption,', 'init_toggle_site_26db0(objects.item0, objects.var0, kSlot455GroupCaption, concmd, menu)', 'init_toggle_site_26db0(objects.item1, objects.var1, kSlot455GroupCaption, concmd, menu)'], 'test_tokens': ['kSlot455Rva == 0x47bfa0 && kSlot455Size == 817', 'section_alone()', 'section_failures()', 'CHECK(creg.atexit_log.size() == 6);', 'CHECK(menu.root.count == 3);', 'CHECK(!r.ok']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, '2 bool variables (tag 0x15bddb8, values 1/0) + 0x26c10 group `Loose Attachment` + two 0x26db0 toggles'))
