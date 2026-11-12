#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 460 (RVA 0x41c340, 857 bytes), batch 6.

bool drawWaypointNode + two unbounded floats (waypoint debug draw): drawWaypointNode (bool, tag 0x15b5750), drawWaypointSegment (float, tag 0x15b5770), drawWaypointSample (float, tag 0x15b5770). Float variables are built by 0x6aa00 (flags 1, value = default, atexit 0x20f0), bool variables by 0x20af0 (atexit 0x20b0); no menu item. Strings are borrowed rodata views.

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
    "slot": 460,
    "base": 0x41c340,
    "size": 857,
    "body_sha256": "47a71a6b6a35bc24bddbdc288836f9e489783bd2e6ded339bf6da0cdfba35b36",
    "pad": "90909090909090",
    "callees": ['concmd_register', 'unbounded_float_ctor', 'float_dtor', 'release_f590', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy', 'concmd_base_ctor', 'bool_dtor', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15b5750],
    "float_tags": [0x15b5770],
    "strings": [
        (0xff08c0, b'drawWaypointNode'),
        (0xff08d1, b'drawWaypointSegment'),
        (0xff08e5, b'drawWaypointSample'),
        (0xff0916, b''),
    ],
    "lengths": [
        ('drawWaypointNode', 0x41c3b3, "48c744242810000000"),
        ('drawWaypointSegment', 0x41c4d8, "48c744240813000000"),
        ('drawWaypointSample', 0x41c5ea, "48c744240812000000"),
    ],
    "pins": [
        ("rip", 0x41c358, 7, 3, 0x15fe518),
        ("rip", 0x41c35f, 7, 3, 0x1594208),
        ("rip", 0x41c366, 7, 3, 0x2ef7cb8),
        ("rip", 0x41c36d, 7, 3, 0xff0916),
        ("rip", 0x41c374, 7, 3, 0xff08c0),
        ("imm", 0x41c389, "b901000000"),
        ("imm", 0x41c397, "4883c310"),
        ("imm", 0x41c3b3, "48c744242810000000"),
        ("rip", 0x41c3c8, 7, 3, 0x2ef7cb8),
        ("imm", 0x41c3cf, "48c744245800000000"),
        ("call", 0x41c3d8, 0x20af0),
        ("rip", 0x41c3dd, 7, 3, 0x15940a0),
        ("rip", 0x41c3e7, 7, 3, 0x2ef7d78),
        ("rip", 0x41c3f2, 7, 3, 0x15b5750),
        ("imm", 0x41c3f9, "4883c010"),
        ("rip", 0x41c3fd, 7, 3, 0x2ef7cb8),
        ("rip", 0x41c404, 7, 3, 0x2ef7d18),
        ("rip", 0x41c40b, 10, 2, 0x2ef7d21),
        ("imm", 0x41c40b, "c7050cb9ad0200000000"),
        ("rip", 0x41c415, 8, 4, 0x2ef7d28),
        ("rip", 0x41c41d, 11, 3, 0x2ef7d58),
        ("imm", 0x41c41d, "48c70530b9ad0200000000"),
        ("rip", 0x41c428, 7, 3, 0x2ef7d68),
        ("rip", 0x41c42f, 11, 3, 0x2ef7d70),
        ("imm", 0x41c42f, "48c70536b9ad0200000000"),
        ("call", 0x41c43a, 0xf590),
        ("rip", 0x41c442, 7, 3, 0x2ef7cb8),
        ("rip", 0x41c449, 7, 2, 0x2ef7d78),
        ("imm", 0x41c449, "c60528b9ad0200"),
        ("rip", 0x41c450, 7, 2, 0x2ef7d20),
        ("imm", 0x41c450, "c605c9b8ad0200"),
        ("call", 0x41c457, 0x20d60),
        ("call", 0x41c45f, 0xf590),
        ("rip", 0x41c464, 7, 3, 0x20b0),
        ("rip", 0x41c46b, 7, 3, 0x1604218),
        ("call", 0x41c475, 0xf8f390),
        ("rip", 0x41c47a, 7, 3, 0x15942a0),
        ("rip", 0x41c481, 7, 3, 0xff08d1),
        ("rip", 0x41c488, 7, 3, 0x2ef7d80),
        ("rip", 0x41c4a5, 7, 3, 0x2ef7e50),
        ("imm", 0x41c4b1, "b901000000"),
        ("imm", 0x41c4b6, "48c744247000000000"),
        ("rip", 0x41c4c8, 7, 3, 0x15b5770),
        ("imm", 0x41c4d8, "48c744240813000000"),
        ("imm", 0x41c4f3, "48c744243800000000"),
        ("imm", 0x41c504, "48c784248800000000000000"),
        ("imm", 0x41c510, "4983c710"),
        ("rip", 0x41c514, 7, 3, 0x2ef7d80),
        ("call", 0x41c51b, 0x6aa00),
        ("call", 0x41c523, 0xf590),
        ("imm", 0x41c533, "ff5018"),
        ("rip", 0x41c539, 7, 3, 0x2ef7d80),
        ("rip", 0x41c540, 10, 2, 0x2ef7e50),
        ("imm", 0x41c540, "c70506b9ad0200000000"),
        ("rip", 0x41c54a, 10, 2, 0x2ef7de8),
        ("imm", 0x41c54a, "c70594b8ad0200000000"),
        ("call", 0x41c554, 0x20d60),
        ("call", 0x41c55c, 0xf590),
        ("rip", 0x41c561, 7, 3, 0x20f0),
        ("rip", 0x41c568, 7, 3, 0x1604218),
        ("call", 0x41c572, 0xf8f390),
        ("rip", 0x41c577, 7, 3, 0x2ef7e58),
        ("rip", 0x41c57e, 7, 3, 0xff0916),
        ("rip", 0x41c58e, 7, 3, 0x15b5770),
        ("rip", 0x41c599, 7, 3, 0xff08e5),
        ("imm", 0x41c5a0, "48c744247000000000"),
        ("imm", 0x41c5b2, "b901000000"),
        ("rip", 0x41c5b7, 7, 3, 0x2ef7e58),
        ("rip", 0x41c5d6, 7, 3, 0x2ef7f28),
        ("imm", 0x41c5ea, "48c744240812000000"),
        ("imm", 0x41c5ff, "48c784248800000000000000"),
        ("imm", 0x41c60b, "48c744243800000000"),
        ("call", 0x41c614, 0x6aa00),
        ("call", 0x41c61c, 0xf590),
        ("imm", 0x41c62c, "ff5018"),
        ("rip", 0x41c632, 7, 3, 0x2ef7e58),
        ("imm", 0x41c639, "c705e5b8ad020000803f"),
        ("imm", 0x41c643, "c70573b8ad020000803f"),
        ("call", 0x41c64d, 0x20d60),
        ("call", 0x41c655, 0xf590),
        ("rip", 0x41c65a, 7, 3, 0x20f0),
        ("rip", 0x41c661, 7, 3, 0x1604218),
        ("call", 0x41c66b, 0xf8f390),
        ("rip", 0x41c670, 7, 3, 0x15fe518),
        ("call", 0x41c693, 0xf8f3a0),
    ],
    "m6_extras": ['bp_atexit_imports', 'bp_object_vtables'],
    "native": {'header': 'native/core/ctor_slot_460_41c340.hpp', 'source': 'native/core/ctor_slot_460_41c340.cpp', 'test': 'native/tests/test_ctor_slot_460.cpp', 'consts': {'kSlot460Rva': 4309824, 'kSlot460Size': 857, 'kSlot460V0NameLength': 16, 'kSlot460V0DescriptionLength': 0, 'kSlot460V0FnTag': 22763344, 'kSlot460V0Value': 0, 'kSlot460V0Rva': 49249464, 'kSlot460V1NameLength': 19, 'kSlot460V1DescriptionLength': 0, 'kSlot460V1FnTag': 22763376, 'kSlot460V1Value': 0, 'kSlot460V1Rva': 49249664, 'kSlot460V2NameLength': 18, 'kSlot460V2DescriptionLength': 0, 'kSlot460V2FnTag': 22763376, 'kSlot460V2Value': 1065353216, 'kSlot460V2Rva': 49249880},
        'header_tokens': ['IDENTITY TAGS', 'kSlot460V0Name[] = "drawWaypointNode"', 'kSlot460V1Name[] = "drawWaypointSegment"', 'kSlot460V2Name[] = "drawWaypointSample"'], 'source_order': ['out.v0 = init_bool_var_static(objects.v0', 'out.v1 = helper_init_unbounded_float_var_site(objects.v1, spec1, concmd);', 'out.v2 = helper_init_unbounded_float_var_site(objects.v2, spec2, concmd);'], 'test_tokens': ['kSlot460Rva == 0x41c340 && kSlot460Size == 857', 'section_alone()', 'section_failures()', 'r.refusal == HelperRefusal::none', 'HelperRefusal::unadmitted_arg_functor', 'check_atexit(creg, 0, 0x20b0, &o.v0)']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'bool drawWaypointNode + two unbounded floats (waypoint debug draw)'))
