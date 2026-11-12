#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 388 (RVA 0x6a8c60, 733 bytes), batch 10 (the survey flagged "1 other indirect call").

Static resolution: the `callq *0x20(%rax)` at 0x6a8eea has rax = `leaq 0x1594dc0(%rip)` (0x6a8e8b, unchanged until the call), so it is the console-command class entry [0x1594dc0 + 0x20] whose RELA record is pinned to 0x20d60
(register-once; the `fcmd_vtable` extra). The 0xf2f0 call (0x6a8e2c) is the 40 byte borrowed name view (hash-pinned, strlen import tied). No indirect target is unresolved; nothing is emulated or executed.

Shape: bool variable cp10EnableBPAnimStateOverrides (0x3017610, tag 0x15cc6f8, value = default 1, empty description), the 0x26ea0 toggle `Enable BP AnimState Overrides` under `Scripted Events` (0x30176d8, flags 0x30,
target the bool variable), and the console command gCP10ExtractCP3EventFrameTables (0x3017780, flags 2, handler tag 0x6a6570 never called, atexit 0x4b0). The handler body is hash-pinned by FDE extent; vtables, handler tags
and destructors are IDENTITY TAGS (recorded, never run). The scripted-events reading is inference from the strings.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 388,
    "base": 0x6a8c60,
    "size": 733,
    "body_sha256": "5ae7df39c4ba3a85376b8f76bb1c22bbef00ef50bcac1f77a6b9203a98005524",
    "pad": "909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'cmd_dtor', 'release_f590', 'menu_toggle_ctor', 'menu_item_dtor', 'bool_dtor', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy'],
    "bool_tags": [0x15cc6f8],
    "float_tags": [],
    "strings": [
        (0x1272a74, b'cp10EnableBPAnimStateOverrides'),
        (0x1272a93, b'Scripted Events'),
        (0x1272aa3, b'Enable BP AnimState Overrides'),
        (0x1272b8d, b'gCP10ExtractCP3EventFrameTables'),
        (0x1272bad, b'Extract CP3 Event FrameTables'),
        (0x1272e76, b''),
    ],
    "lengths": [
        ('cp10EnableBPAnimStateOverrides', 0x6a8ccd, "48c74424281e000000"),
        ('Scripted Events', 0x6a8dd6, "48c74424480f000000"),
        ('Enable BP AnimState Overrides', 0x6a8ddf, "48c74424281d000000"),
        ('Extract CP3 Event FrameTables', 0x6a8e61, "48c74424281d000000"),
    ],
    "pins": [
        ("rip", 0x6a8c78, 7, 3, 0x15fe518),
        ("rip", 0x6a8c7f, 7, 3, 0x1594208),
        ("rip", 0x6a8c86, 7, 3, 0x3017610),
        ("rip", 0x6a8c8d, 7, 3, 0x1272a74),
        ("rip", 0x6a8c94, 7, 3, 0x1272e76),
        ("imm", 0x6a8ca9, "b901000000"),
        ("imm", 0x6a8cb4, "4883c310"),
        ("imm", 0x6a8ccd, "48c74424281e000000"),
        ("rip", 0x6a8ce2, 7, 3, 0x3017610),
        ("imm", 0x6a8ce9, "48c744245800000000"),
        ("call", 0x6a8cf2, 0x20af0),
        ("rip", 0x6a8cf7, 7, 3, 0x15940a0),
        ("rip", 0x6a8d01, 7, 3, 0x30176d0),
        ("rip", 0x6a8d0c, 7, 3, 0x15cc6f8),
        ("imm", 0x6a8d13, "4883c010"),
        ("rip", 0x6a8d17, 7, 3, 0x3017610),
        ("rip", 0x6a8d1e, 7, 3, 0x3017670),
        ("rip", 0x6a8d25, 10, 2, 0x3017679),
        ("imm", 0x6a8d25, "c7054ae9960200000000"),
        ("rip", 0x6a8d2f, 8, 4, 0x3017680),
        ("rip", 0x6a8d37, 11, 3, 0x30176b0),
        ("imm", 0x6a8d37, "48c7056ee9960200000000"),
        ("rip", 0x6a8d42, 7, 3, 0x30176c0),
        ("rip", 0x6a8d49, 11, 3, 0x30176c8),
        ("imm", 0x6a8d49, "48c70574e9960200000000"),
        ("call", 0x6a8d54, 0xf590),
        ("rip", 0x6a8d5c, 7, 3, 0x3017610),
        ("rip", 0x6a8d63, 7, 2, 0x30176d0),
        ("imm", 0x6a8d63, "c60566e9960201"),
        ("rip", 0x6a8d6a, 7, 2, 0x3017678),
        ("imm", 0x6a8d6a, "c60507e9960201"),
        ("call", 0x6a8d71, 0x20d60),
        ("call", 0x6a8d79, 0xf590),
        ("rip", 0x6a8d7e, 7, 3, 0x1604218),
        ("rip", 0x6a8d85, 7, 3, 0x20b0),
        ("call", 0x6a8d92, 0xf8f390),
        ("rip", 0x6a8d97, 7, 3, 0x1272aa3),
        ("rip", 0x6a8d9e, 7, 3, 0x30176d8),
        ("rip", 0x6a8daf, 7, 3, 0x1272a93),
        ("imm", 0x6a8dba, "41b830000000"),
        ("imm", 0x6a8dd6, "48c74424480f000000"),
        ("imm", 0x6a8ddf, "48c74424281d000000"),
        ("call", 0x6a8df4, 0x26ea0),
        ("call", 0x6a8dfc, 0xf590),
        ("call", 0x6a8e04, 0xf590),
        ("rip", 0x6a8e09, 7, 3, 0x272d0),
        ("call", 0x6a8e16, 0xf8f390),
        ("rip", 0x6a8e20, 7, 3, 0x1272b8d),
        ("call", 0x6a8e2c, 0xf2f0),
        ("rip", 0x6a8e31, 8, 4, 0x15fe5e0),
        ("rip", 0x6a8e39, 7, 3, 0x1272bad),
        ("rip", 0x6a8e44, 7, 3, 0x3017780),
        ("imm", 0x6a8e50, "b902000000"),
        ("imm", 0x6a8e61, "48c74424281d000000"),
        ("rip", 0x6a8e74, 7, 3, 0x6a6570),
        ("call", 0x6a8e86, 0x20af0),
        ("rip", 0x6a8e8b, 7, 3, 0x1594dc0),
        ("rip", 0x6a8e9b, 7, 3, 0x3017780),
        ("rip", 0x6a8ea2, 8, 4, 0x30177e0),
        ("imm", 0x6a8eaa, "c4e37d19c001"),
        ("imm", 0x6a8eb0, "c4e3f916c101"),
        ("rip", 0x6a8ebb, 7, 3, 0x30177e0),
        ("rip", 0x6a8ec2, 7, 3, 0x30177e0),
        ("rip", 0x6a8ec9, 7, 3, 0x3017780),
        ("rip", 0x6a8ed7, 8, 4, 0x3017800),
        ("rip", 0x6a8edf, 11, 3, 0x3017810),
        ("imm", 0x6a8edf, "48c70526e9960200000000"),
        ("imm", 0x6a8eea, "ff5020"),
        ("call", 0x6a8ef2, 0xf590),
        ("call", 0x6a8efc, 0xf590),
        ("rip", 0x6a8f01, 7, 3, 0x4b0),
        ("rip", 0x6a8f08, 7, 3, 0x1604218),
        ("call", 0x6a8f12, 0xf8f390),
        ("rip", 0x6a8f17, 7, 3, 0x15fe518),
        ("call", 0x6a8f37, 0xf8f3a0),
        ("imm", 0x6a8e8b, "488d052ebfee00"),
        ("imm", 0x6a8e74, "488d05f5d6ffff"),
        ("imm", 0x6a8d0c, "488d0de539f200"),
    ],
    "m6_callees": {
        'handler_tag_6a6570': (0x6a6570, 4473, "5b6a7d2eb2d6e5c97f652857b6725f5ef5b964d6146d989a310fa95a8748c6f6"),
        'name_view_f2f0': (0xf2f0, 40, "0173e3a3f11a8aa020b87731d2eef5026621d6f1fa0967581b0dab3df646973b"),
    },
    "m6_extras": ['bp_atexit_imports', 'bp_strlen_import', 'fcmd_vtable'],
    "native": {'header': 'native/core/ctor_slot_388_6a8c60.hpp', 'source': 'native/core/ctor_slot_388_6a8c60.cpp', 'test': 'native/tests/test_ctor_slot_388.cpp', 'consts': {'kSlot388Rva': 6982752, 'kSlot388Size': 733, 'kSlot388BoolFnTag': 22857464, 'kSlot388HandlerTag': 6972784, 'kSlot388ItemFlags': 48, 'kSlot388BoolNameLength': 30, 'kSlot388BoolValue': 1, 'kSlot388ToggleParentLength': 15, 'kSlot388ToggleTextLength': 29, 'kSlot388CommandDescriptionLength': 29, 'kSlot388BoolRva': 50427408, 'kSlot388ItemRva': 50427608, 'kSlot388CommandRva': 50427776},
        'header_tokens': ['kSlot388BoolName[] = "cp10EnableBPAnimStateOverrides"', 'kSlot388ToggleParent[] = "Scripted Events"', 'kSlot388ToggleText[] = "Enable BP AnimState Overrides"', 'kSlot388CommandName[] = "gCP10ExtractCP3EventFrameTables"', 'kSlot388CommandDescription[] = "Extract CP3 Event FrameTables"', 'IDENTITY TAGS'], 'source_order': ['lane_m6::init_bool_var_literal(objects.bvar', 'family_toggle_item_26ea0(objects.item', 'init_fconcmd_object(objects.command, cmd, concmd)'], 'test_tokens': ['kSlot388Rva == 0x6a8c60 && kSlot388Size == 733', 'section_alone()', 'section_failures()', 'r.ok && r.rows_done == 3', 'creg.pending.size() == 2', 'creg.atexit_log.size() == 4', 'check_atexit(creg, 0, 0x20b0, &o.bvar)', 'check_atexit(creg, 2, 0x272d0, &o.item)', 'check_atexit(creg, 3, 0x4b0, &o.command)', 'o.item.range_min == 1.0f && o.item.range_max == 10.0f', 'menu.parent.parent = &token']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'bool var + 0x26ea0 toggle + console command gCP10ExtractCP3EventFrameTables (*0x20(%rax) resolved to 0x20d60)'))
