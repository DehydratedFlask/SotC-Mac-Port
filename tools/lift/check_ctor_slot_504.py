#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 504 (RVA 0x23f4d0, 562 bytes), batch 6.

The UNBOUNDED float console variable `uiOverrideSafeFrame` (0x6aa00, flags 1, value = default = 0.0f, caller functor tag 0x159ba68, atexit 0x20f0) followed by the bool console
variable `uiShowSafeFrame` (0x20af0, tag 0x159ba88 stored at +0xb0, value 0 / default 0, atexit 0x20b0). Both carry borrowed rodata descriptions; no menu item.

Helper-callee tie: the caller functor tag 0x159ba68 is one of the 68 float-family class tables (slots {0xc4a0, 0xc4c0, 0xc4e0, 0xc4f0}); the native slot reaches 0x6aa00 through
helper_6aa00_unbounded_float and requires HelperRefusal::none. tools/lift/check_helper_6aa00.py pins the same call site (call 0x23f59e).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited driver runs a fail-closed mutation self-test;
--tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 504,
    "base": 0x23f4d0,
    "size": 562,
    "body_sha256": "46dc649502bece49df2b8bfbfce883785c00f3775a02475e6cf9b95faf7dffd4",
    "pad": "9090909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'float_dtor', 'unbounded_float_ctor', 'release_f590', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x159ba88],
    "float_tags": [0x159ba68],
    "strings": [
        (0xfc11c0, b'uiOverrideSafeFrame'),
        (0xfc11d4, b"If non zero this value will be used for the safe frame percentage. Sony's config app tops out at 0.9."),
        (0xfc123a, b'uiShowSafeFrame'),
        (0xfc124a, b'If true the safe frame will be overlaid onto the screen for reference'),
    ],
    "lengths": [
        ('uiOverrideSafeFrame', 0x23f545, "48c744242813000000"),
        ("If non zero this value will be used for the safe frame percentage. Sony's config app tops out at 0.9.", 0x23f54e, "48c744240865000000"),
        ('uiShowSafeFrame', 0x23f630, "48c74424480f000000"),
        ('If true the safe frame will be overlaid onto the screen for reference', 0x23f644, "48c744242845000000"),
    ],
    "pins": [
        ("rip", 0x23f4e8, 7, 3, 0x15fe518),
        ("rip", 0x23f4ef, 7, 3, 0x15942a0),
        ("rip", 0x23f4f6, 7, 3, 0x2eea2d8),
        ("rip", 0x23f501, 7, 3, 0xfc11c0),
        ("rip", 0x23f50c, 7, 3, 0xfc11d4),
        ("imm", 0x23f51d, "b901000000"),
        ("imm", 0x23f52d, "4983c510"),
        ("imm", 0x23f545, "48c744242813000000"),
        ("imm", 0x23f54e, "48c744240865000000"),
        ("rip", 0x23f567, 7, 3, 0x159ba68),
        ("imm", 0x23f578, "48c744247000000000"),
        ("imm", 0x23f592, "48c784248800000000000000"),
        ("call", 0x23f59e, 0x6aa00),
        ("imm", 0x23f5ae, "ff5018"),
        ("imm", 0x23f5b7, "c783d000000000000000"),
        ("imm", 0x23f5c1, "c7436800000000"),
        ("call", 0x23f5c8, 0x20d60),
        ("call", 0x23f5d0, 0xf590),
        ("call", 0x23f5d8, 0xf590),
        ("rip", 0x23f5dd, 7, 3, 0x1604218),
        ("rip", 0x23f5e4, 7, 3, 0x20f0),
        ("call", 0x23f5f1, 0xf8f390),
        ("rip", 0x23f5f6, 7, 3, 0x1594208),
        ("rip", 0x23f5fd, 7, 3, 0x2eea3b0),
        ("rip", 0x23f604, 7, 3, 0xfc123a),
        ("imm", 0x23f619, "b901000000"),
        ("rip", 0x23f623, 7, 3, 0xfc124a),
        ("imm", 0x23f630, "48c74424480f000000"),
        ("imm", 0x23f644, "48c744242845000000"),
        ("imm", 0x23f653, "4983c510"),
        ("call", 0x23f65d, 0x20af0),
        ("rip", 0x23f670, 7, 3, 0x159ba88),
        ("imm", 0x23f67b, "c7436900000000"),
        ("imm", 0x23f687, "48c783a000000000000000"),
        ("imm", 0x23f699, "48c783b800000000000000"),
        ("imm", 0x23f6a7, "c683c000000000"),
        ("imm", 0x23f6ae, "c6436800"),
        ("call", 0x23f6b2, 0x20d60),
        ("call", 0x23f6ba, 0xf590),
        ("call", 0x23f6c2, 0xf590),
        ("rip", 0x23f6c7, 7, 3, 0x20b0),
        ("call", 0x23f6d4, 0xf8f390),
        ("rip", 0x23f6d9, 7, 3, 0x15fe518),
        ("call", 0x23f6fc, 0xf8f3a0),
    ],
    "m6_extras": ['bp_atexit_imports', 'bp_object_vtables'],
    "native": {'header': 'native/core/ctor_slot_504_23f4d0.hpp', 'source': 'native/core/ctor_slot_504_23f4d0.cpp', 'test': 'native/tests/test_ctor_slot_504.cpp', 'consts': {'kSlot504Rva': 2356432, 'kSlot504Size': 562, 'kSlot504FloatFnTag': 22657640, 'kSlot504BoolFnTag': 22657672, 'kSlot504FloatNameLength': 19, 'kSlot504FloatDescriptionLength': 101, 'kSlot504BoolNameLength': 15, 'kSlot504BoolDescriptionLength': 69, 'kSlot504FloatRva': 49193688, 'kSlot504BoolRva': 49193904},
        'header_tokens': ['kSlot504FloatName[] = "uiOverrideSafeFrame"', 'kSlot504BoolName[] = "uiShowSafeFrame"', 'IDENTITY TAGS'], 'source_order': ['fv.base.vptr = kRangedFloatFinalVptr;', 'helper_6aa00_unbounded_float(fv, fname, &fv.value, 1, args, fdesc)', 'out.float_destroy_ok = ctor.arg_destroy_ok;', 'out.float_registration = register_concmd_20d60(fv.base, concmd);', 'release_inert_view_f590(fdesc)', 'release_inert_view_f590(fname)', 'concmd.atexit_log.record(kRangedFloatDtorRva, &fv)', 'lane_m4::init_bool_var_late_release(objects.bool_var, bname, bdesc, kSlot504BoolFnTag, concmd, 0, 0)'], 'test_tokens': ['kSlot504Rva == 0x23f4d0 && kSlot504Size == 562', 'section_alone()', 'section_failures()', 'r.refusal == HelperRefusal::none', 'check_atexit(creg, 0, 0x20f0, &o.float_var)', 'check_atexit(creg, 1, 0x20b0, &o.bool_var)', 'HelperRefusal::unadmitted_arg_functor']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'unbounded float var uiOverrideSafeFrame (tag 0x159ba68 via the 6aa00 family wrapper) + bool var uiShowSafeFrame (tag 0x159ba88)'))
