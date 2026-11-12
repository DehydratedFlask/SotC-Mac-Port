#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 397 (RVA 0x64d760, 1036 bytes), batch 6.

four unbounded float console variables (player acceleration scalars): playerAccelerationScalar30Hz (float, tag 0x15cc528), playerAccelerationScalarEasyMode30Hz (float, tag 0x15cc528), playerAccelerationScalar60Hz (float, tag 0x15cc528), playerAccelerationScalarEasyMode60Hz (float, tag 0x15cc528). Float variables are built by 0x6aa00 (flags 1, value = default, atexit 0x20f0), bool variables by 0x20af0 (atexit 0x20b0); no menu item. Strings are borrowed rodata views.

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
    "slot": 397,
    "base": 0x64d760,
    "size": 1036,
    "body_sha256": "a7fc208561589933de8759f0a796672dfa66fb74bde273fcfeff79cb0b5140b1",
    "pad": "90909090",
    "callees": ['concmd_register', 'unbounded_float_ctor', 'float_dtor', 'release_f590', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy'],
    "bool_tags": [],
    "float_tags": [0x15cc528],
    "strings": [
        (0x126e7a0, b'playerAccelerationScalar30Hz'),
        (0x126e7bd, b'playerAccelerationScalarEasyMode30Hz'),
        (0x126e7e2, b'playerAccelerationScalar60Hz'),
        (0x126e7ff, b'playerAccelerationScalarEasyMode60Hz'),
        (0x126e829, b''),
    ],
    "lengths": [
        ('playerAccelerationScalar30Hz', 0x64d7d4, "48c74424081c000000"),
        ('playerAccelerationScalarEasyMode30Hz', 0x64d8d1, "48c744240824000000"),
        ('playerAccelerationScalar60Hz', 0x64d9c3, "48c74424081c000000"),
        ('playerAccelerationScalarEasyMode60Hz', 0x64daa3, "48c744240824000000"),
    ],
    "pins": [
        ("rip", 0x64d778, 7, 3, 0x15fe518),
        ("rip", 0x64d77f, 7, 3, 0x15942a0),
        ("rip", 0x64d786, 7, 3, 0x300fea8),
        ("rip", 0x64d78d, 7, 3, 0x126e7a0),
        ("rip", 0x64d79d, 7, 3, 0x15cc528),
        ("imm", 0x64d7b1, "b901000000"),
        ("imm", 0x64d7c1, "4983c710"),
        ("imm", 0x64d7d4, "48c74424081c000000"),
        ("rip", 0x64d7e3, 7, 3, 0x126e829),
        ("imm", 0x64d7ee, "48c744247000000000"),
        ("imm", 0x64d81d, "48c784248800000000000000"),
        ("imm", 0x64d829, "48c744243800000000"),
        ("call", 0x64d832, 0x6aa00),
        ("call", 0x64d83a, 0xf590),
        ("imm", 0x64d84a, "ff5018"),
        ("imm", 0x64d853, "c783d00000000000403f"),
        ("imm", 0x64d85d, "c743680000403f"),
        ("call", 0x64d864, 0x20d60),
        ("call", 0x64d86c, 0xf590),
        ("rip", 0x64d871, 7, 3, 0x20f0),
        ("rip", 0x64d878, 7, 3, 0x1604218),
        ("call", 0x64d882, 0xf8f390),
        ("rip", 0x64d887, 7, 3, 0x300ff80),
        ("rip", 0x64d88e, 7, 3, 0x126e7bd),
        ("rip", 0x64d8a6, 7, 3, 0x15cc528),
        ("imm", 0x64d8b2, "b901000000"),
        ("imm", 0x64d8b7, "48c744247000000000"),
        ("rip", 0x64d8c4, 7, 3, 0x126e829),
        ("imm", 0x64d8d1, "48c744240824000000"),
        ("imm", 0x64d8ee, "48c784248800000000000000"),
        ("imm", 0x64d905, "48c744243800000000"),
        ("call", 0x64d91b, 0x6aa00),
        ("call", 0x64d923, 0xf590),
        ("imm", 0x64d935, "ff5018"),
        ("imm", 0x64d93e, "c783d00000001f852b3f"),
        ("imm", 0x64d948, "c743681f852b3f"),
        ("call", 0x64d94f, 0x20d60),
        ("call", 0x64d957, 0xf590),
        ("rip", 0x64d95c, 7, 3, 0x1604218),
        ("rip", 0x64d963, 7, 3, 0x20f0),
        ("call", 0x64d970, 0xf8f390),
        ("rip", 0x64d975, 7, 3, 0x3010058),
        ("rip", 0x64d980, 7, 3, 0x126e7e2),
        ("imm", 0x64d987, "48c744247000000000"),
        ("imm", 0x64d99e, "b901000000"),
        ("rip", 0x64d9b9, 7, 3, 0x126e829),
        ("imm", 0x64d9c3, "48c74424081c000000"),
        ("imm", 0x64d9d2, "48c784248800000000000000"),
        ("imm", 0x64d9ec, "48c744243800000000"),
        ("call", 0x64da02, 0x6aa00),
        ("call", 0x64da0a, 0xf590),
        ("imm", 0x64da1c, "ff5018"),
        ("imm", 0x64da25, "c783d000000085eb113f"),
        ("imm", 0x64da2f, "c7436885eb113f"),
        ("call", 0x64da36, 0x20d60),
        ("call", 0x64da3e, 0xf590),
        ("rip", 0x64da43, 7, 3, 0x20f0),
        ("call", 0x64da50, 0xf8f390),
        ("rip", 0x64da55, 7, 3, 0x3010130),
        ("rip", 0x64da5c, 7, 3, 0x126e7ff),
        ("imm", 0x64da70, "48c744247000000000"),
        ("imm", 0x64da82, "b901000000"),
        ("rip", 0x64da90, 7, 3, 0x15cc528),
        ("imm", 0x64daa3, "48c744240824000000"),
        ("rip", 0x64daba, 7, 3, 0x126e829),
        ("imm", 0x64dac1, "48c784248800000000000000"),
        ("imm", 0x64dad8, "48c744243800000000"),
        ("call", 0x64daee, 0x6aa00),
        ("call", 0x64daf6, 0xf590),
        ("imm", 0x64db06, "ff5018"),
        ("imm", 0x64db0f, "c783d000000085eb113f"),
        ("imm", 0x64db19, "c7436885eb113f"),
        ("call", 0x64db20, 0x20d60),
        ("call", 0x64db28, 0xf590),
        ("rip", 0x64db2d, 7, 3, 0x20f0),
        ("rip", 0x64db34, 7, 3, 0x1604218),
        ("call", 0x64db3e, 0xf8f390),
        ("rip", 0x64db43, 7, 3, 0x15fe518),
        ("call", 0x64db66, 0xf8f3a0),
    ],
    "m6_extras": ['bp_atexit_imports', 'bp_object_vtables'],
    "native": {'header': 'native/core/ctor_slot_397_64d760.hpp', 'source': 'native/core/ctor_slot_397_64d760.cpp', 'test': 'native/tests/test_ctor_slot_397.cpp', 'consts': {'kSlot397Rva': 6608736, 'kSlot397Size': 1036, 'kSlot397V0NameLength': 28, 'kSlot397V0DescriptionLength': 0, 'kSlot397V0FnTag': 22857000, 'kSlot397V0Value': 1061158912, 'kSlot397V0Rva': 50396840, 'kSlot397V1NameLength': 36, 'kSlot397V1DescriptionLength': 0, 'kSlot397V1FnTag': 22857000, 'kSlot397V1Value': 1059816735, 'kSlot397V1Rva': 50397056, 'kSlot397V2NameLength': 28, 'kSlot397V2DescriptionLength': 0, 'kSlot397V2FnTag': 22857000, 'kSlot397V2Value': 1058139013, 'kSlot397V2Rva': 50397272, 'kSlot397V3NameLength': 36, 'kSlot397V3DescriptionLength': 0, 'kSlot397V3FnTag': 22857000, 'kSlot397V3Value': 1058139013, 'kSlot397V3Rva': 50397488},
        'header_tokens': ['IDENTITY TAGS', 'kSlot397V0Name[] = "playerAccelerationScalar30Hz"', 'kSlot397V1Name[] = "playerAccelerationScalarEasyMode30Hz"', 'kSlot397V2Name[] = "playerAccelerationScalar60Hz"', 'kSlot397V3Name[] = "playerAccelerationScalarEasyMode60Hz"'], 'source_order': ['out.v0 = helper_init_unbounded_float_var_site(objects.v0, spec0, concmd);', 'out.v1 = helper_init_unbounded_float_var_site(objects.v1, spec1, concmd);', 'out.v2 = helper_init_unbounded_float_var_site(objects.v2, spec2, concmd);', 'out.v3 = helper_init_unbounded_float_var_site(objects.v3, spec3, concmd);'], 'test_tokens': ['kSlot397Rva == 0x64d760 && kSlot397Size == 1036', 'section_alone()', 'section_failures()', 'r.refusal == HelperRefusal::none', 'HelperRefusal::unadmitted_arg_functor', 'check_atexit(creg, 0, 0x20f0, &o.v0)']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'four unbounded float console variables (player acceleration scalars)'))
