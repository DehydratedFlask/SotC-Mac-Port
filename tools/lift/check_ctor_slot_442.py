#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 442 (RVA 0x4e3760, 779 bytes), batch 6.

bool + float + bool grip-fill test variables: cp10_testUI_testGripFillPercent (bool, tag 0x15c27d0), cp10_testUI_testGripFillPercentSpeed (float, tag 0x15c27f0), cp10_testUI_testGripAttackCircle (bool, tag 0x15c27d0). Float variables are built by 0x6aa00 (flags 1, value = default, atexit 0x20f0), bool variables by 0x20af0 (atexit 0x20b0); no menu item. Strings are borrowed rodata views.

Helper-callee tie: every float caller functor tag is a member of the 68 float-family class tables (slots {0xc4a0, 0xc4c0, 0xc4e0, 0xc4f0}); the native slot reaches 0x6aa00 through
helper_6aa00_unbounded_float and requires HelperRefusal::none. tools/lift/check_helper_6aa00.py pins the same call sites.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited driver runs a fail-closed mutation self-test;
--tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 442,
    "base": 0x4e3760,
    "size": 779,
    "body_sha256": "1a7872289081393b3909c2213060806ce6117fa5aee40254e8a16e915cdb0aa8",
    "pad": "9090909090",
    "callees": ['concmd_register', 'unbounded_float_ctor', 'float_dtor', 'release_f590', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy', 'concmd_base_ctor', 'bool_dtor', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15c27d0],
    "float_tags": [0x15c27f0],
    "strings": [
        (0x1008c60, b'cp10_testUI_testGripFillPercent'),
        (0x1008c80, b'Automatically cycles through fill percentages.'),
        (0x1008caf, b'cp10_testUI_testGripFillPercentSpeed'),
        (0x1008cd4, b'How fast to automatically cycle through fill percentages.'),
        (0x1008d0e, b'cp10_testUI_testGripAttackCircle'),
        (0x1008d2f, b'Automatically cycles through attack circle charging.'),
    ],
    "lengths": [
        ('cp10_testUI_testGripFillPercent', 0x4e37d0, "48c74424481f000000"),
        ('Automatically cycles through fill percentages.', 0x4e37d9, "48c74424282e000000"),
        ('cp10_testUI_testGripFillPercentSpeed', 0x4e38b2, "48c744242824000000"),
        ('How fast to automatically cycle through fill percentages.', 0x4e38cb, "48c744240839000000"),
        ('cp10_testUI_testGripAttackCircle', 0x4e399d, "48c744244820000000"),
        ('Automatically cycles through attack circle charging.', 0x4e39b1, "48c744242834000000"),
    ],
    "pins": [
        ("rip", 0x4e3778, 7, 3, 0x15fe518),
        ("rip", 0x4e377f, 7, 3, 0x1594208),
        ("rip", 0x4e3786, 7, 3, 0x2efbc90),
        ("rip", 0x4e378d, 7, 3, 0x1008c60),
        ("rip", 0x4e3794, 7, 3, 0x1008c80),
        ("imm", 0x4e37a9, "b901000000"),
        ("imm", 0x4e37b1, "4983c510"),
        ("imm", 0x4e37d0, "48c74424481f000000"),
        ("imm", 0x4e37d9, "48c74424282e000000"),
        ("call", 0x4e37f1, 0x20af0),
        ("rip", 0x4e3804, 7, 3, 0x15c27d0),
        ("imm", 0x4e380f, "c7436900000000"),
        ("imm", 0x4e381b, "48c783a000000000000000"),
        ("imm", 0x4e382d, "48c783b800000000000000"),
        ("imm", 0x4e383b, "c683c000000000"),
        ("imm", 0x4e3842, "c6436800"),
        ("call", 0x4e3846, 0x20d60),
        ("call", 0x4e384e, 0xf590),
        ("call", 0x4e3856, 0xf590),
        ("rip", 0x4e385b, 7, 3, 0x20b0),
        ("rip", 0x4e3862, 7, 3, 0x1604218),
        ("call", 0x4e386c, 0xf8f390),
        ("rip", 0x4e3871, 7, 3, 0x15942a0),
        ("rip", 0x4e3878, 7, 3, 0x2efbd58),
        ("rip", 0x4e387f, 7, 3, 0x1008caf),
        ("imm", 0x4e3898, "b901000000"),
        ("imm", 0x4e389d, "48c744247000000000"),
        ("rip", 0x4e38ab, 7, 3, 0x1008cd4),
        ("imm", 0x4e38b2, "48c744242824000000"),
        ("imm", 0x4e38cb, "48c744240839000000"),
        ("rip", 0x4e38de, 7, 3, 0x15c27f0),
        ("imm", 0x4e38f3, "48c784248800000000000000"),
        ("imm", 0x4e38ff, "4983c410"),
        ("call", 0x4e3910, 0x6aa00),
        ("imm", 0x4e3922, "ff5018"),
        ("imm", 0x4e392b, "c783d00000000000803f"),
        ("imm", 0x4e3935, "c743680000803f"),
        ("call", 0x4e393c, 0x20d60),
        ("call", 0x4e3944, 0xf590),
        ("call", 0x4e394c, 0xf590),
        ("rip", 0x4e3951, 7, 3, 0x1604218),
        ("rip", 0x4e3958, 7, 3, 0x20f0),
        ("call", 0x4e3965, 0xf8f390),
        ("rip", 0x4e396a, 7, 3, 0x2efbe30),
        ("rip", 0x4e3971, 7, 3, 0x1008d0e),
        ("imm", 0x4e3986, "b901000000"),
        ("rip", 0x4e3990, 7, 3, 0x1008d2f),
        ("imm", 0x4e399d, "48c744244820000000"),
        ("imm", 0x4e39b1, "48c744242834000000"),
        ("call", 0x4e39c6, 0x20af0),
        ("rip", 0x4e39d9, 7, 3, 0x15c27d0),
        ("imm", 0x4e39e4, "c7436900000000"),
        ("imm", 0x4e39f0, "48c783a000000000000000"),
        ("imm", 0x4e3a02, "48c783b800000000000000"),
        ("imm", 0x4e3a10, "c683c000000000"),
        ("imm", 0x4e3a17, "c6436800"),
        ("call", 0x4e3a1b, 0x20d60),
        ("call", 0x4e3a23, 0xf590),
        ("call", 0x4e3a2b, 0xf590),
        ("rip", 0x4e3a30, 7, 3, 0x20b0),
        ("call", 0x4e3a3d, 0xf8f390),
        ("rip", 0x4e3a42, 7, 3, 0x15fe518),
        ("call", 0x4e3a65, 0xf8f3a0),
    ],
    "m6_extras": ['bp_atexit_imports', 'bp_object_vtables'],
    "native": {'header': 'native/core/ctor_slot_442_4e3760.hpp', 'source': 'native/core/ctor_slot_442_4e3760.cpp', 'test': 'native/tests/test_ctor_slot_442.cpp', 'consts': {'kSlot442Rva': 5125984, 'kSlot442Size': 779, 'kSlot442V0NameLength': 31, 'kSlot442V0DescriptionLength': 46, 'kSlot442V0FnTag': 22816720, 'kSlot442V0Value': 0, 'kSlot442V0Rva': 49265808, 'kSlot442V1NameLength': 36, 'kSlot442V1DescriptionLength': 57, 'kSlot442V1FnTag': 22816752, 'kSlot442V1Value': 1065353216, 'kSlot442V1Rva': 49266008, 'kSlot442V2NameLength': 32, 'kSlot442V2DescriptionLength': 52, 'kSlot442V2FnTag': 22816720, 'kSlot442V2Value': 0, 'kSlot442V2Rva': 49266224},
        'header_tokens': ['IDENTITY TAGS', 'kSlot442V0Name[] = "cp10_testUI_testGripFillPercent"', 'kSlot442V0Description[] = "Automatically cycles through fill percentages."', 'kSlot442V1Name[] = "cp10_testUI_testGripFillPercentSpeed"', 'kSlot442V1Description[] = "How fast to automatically cycle through fill percentages."', 'kSlot442V2Name[] = "cp10_testUI_testGripAttackCircle"', 'kSlot442V2Description[] = "Automatically cycles through attack circle charging."'], 'source_order': ['out.v0 = lane_m4::init_bool_var_late_release(objects.v0', 'out.v1 = helper_init_unbounded_float_var_site(objects.v1, spec1, concmd);', 'out.v2 = lane_m4::init_bool_var_late_release(objects.v2'], 'test_tokens': ['kSlot442Rva == 0x4e3760 && kSlot442Size == 779', 'section_alone()', 'section_failures()', 'r.refusal == HelperRefusal::none', 'HelperRefusal::unadmitted_arg_functor', 'check_atexit(creg, 0, 0x20b0, &o.v0)']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'bool + float + bool grip-fill test variables'))
