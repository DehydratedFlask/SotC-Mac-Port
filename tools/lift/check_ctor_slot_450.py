#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 450 (RVA 0x4af4b0, 808 bytes), batch 6.

three unbounded float console variables (UI logo / main-menu timings): uiLogoFadeInFromWhiteTime (float, tag 0x15c05f0), uiLogoFadeOutTime (float, tag 0x15c05f0), uiMainMenuStartDelay (float, tag 0x15c05f0). Float variables are built by 0x6aa00 (flags 1, value = default, atexit 0x20f0), bool variables by 0x20af0 (atexit 0x20b0); no menu item. Strings are borrowed rodata views.

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
    "slot": 450,
    "base": 0x4af4b0,
    "size": 808,
    "body_sha256": "3cbbb9b3dba7a94dd670ed8a68ab84ce9649d263e3112503b773e8cad78ff92f",
    "pad": "9090909090909090",
    "callees": ['concmd_register', 'unbounded_float_ctor', 'float_dtor', 'release_f590', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy'],
    "bool_tags": [],
    "float_tags": [0x15c05f0],
    "strings": [
        (0x1000e60, b'uiLogoFadeInFromWhiteTime'),
        (0x1000e7a, b'uiLogoFadeOutTime'),
        (0x1000e8c, b'uiMainMenuStartDelay'),
        (0x1001040, b''),
    ],
    "lengths": [
        ('uiLogoFadeInFromWhiteTime', 0x4af545, "48c744240819000000"),
        ('uiLogoFadeOutTime', 0x4af621, "48c744240811000000"),
        ('uiMainMenuStartDelay', 0x4af70f, "48c744240814000000"),
    ],
    "pins": [
        ("rip", 0x4af4c8, 7, 3, 0x15fe518),
        ("rip", 0x4af4cf, 7, 3, 0x15942a0),
        ("rip", 0x4af4d6, 7, 3, 0x2efa178),
        ("rip", 0x4af4dd, 7, 3, 0x1000e60),
        ("rip", 0x4af4ed, 7, 3, 0x15c05f0),
        ("imm", 0x4af501, "b901000000"),
        ("imm", 0x4af511, "4983c710"),
        ("rip", 0x4af524, 7, 3, 0x1001040),
        ("imm", 0x4af52b, "48c744247000000000"),
        ("imm", 0x4af545, "48c744240819000000"),
        ("imm", 0x4af557, "48c784248800000000000000"),
        ("imm", 0x4af575, "48c744243800000000"),
        ("call", 0x4af57e, 0x6aa00),
        ("call", 0x4af586, 0xf590),
        ("imm", 0x4af596, "ff5018"),
        ("imm", 0x4af59f, "c783d00000000000803f"),
        ("imm", 0x4af5a9, "c743680000803f"),
        ("call", 0x4af5b0, 0x20d60),
        ("call", 0x4af5b8, 0xf590),
        ("rip", 0x4af5bd, 7, 3, 0x20f0),
        ("rip", 0x4af5c4, 7, 3, 0x1604218),
        ("call", 0x4af5ce, 0xf8f390),
        ("rip", 0x4af5d3, 7, 3, 0x2efa250),
        ("rip", 0x4af5da, 7, 3, 0x1000e7a),
        ("imm", 0x4af5ee, "48c744247000000000"),
        ("imm", 0x4af600, "b901000000"),
        ("rip", 0x4af60e, 7, 3, 0x15c05f0),
        ("imm", 0x4af621, "48c744240811000000"),
        ("rip", 0x4af638, 7, 3, 0x1001040),
        ("imm", 0x4af63f, "48c784248800000000000000"),
        ("imm", 0x4af656, "48c744243800000000"),
        ("call", 0x4af66c, 0x6aa00),
        ("call", 0x4af674, 0xf590),
        ("imm", 0x4af684, "ff5018"),
        ("imm", 0x4af68d, "c783d00000000000c03f"),
        ("imm", 0x4af697, "c743680000c03f"),
        ("call", 0x4af69e, 0x20d60),
        ("call", 0x4af6a6, 0xf590),
        ("rip", 0x4af6ab, 7, 3, 0x20f0),
        ("rip", 0x4af6b2, 7, 3, 0x1604218),
        ("call", 0x4af6bc, 0xf8f390),
        ("rip", 0x4af6c1, 7, 3, 0x2efa328),
        ("rip", 0x4af6c8, 7, 3, 0x1000e8c),
        ("imm", 0x4af6dc, "48c744247000000000"),
        ("imm", 0x4af6ee, "b901000000"),
        ("rip", 0x4af6fc, 7, 3, 0x15c05f0),
        ("imm", 0x4af70f, "48c744240814000000"),
        ("rip", 0x4af726, 7, 3, 0x1001040),
        ("imm", 0x4af72d, "48c784248800000000000000"),
        ("imm", 0x4af744, "48c744243800000000"),
        ("call", 0x4af75a, 0x6aa00),
        ("call", 0x4af762, 0xf590),
        ("imm", 0x4af772, "ff5018"),
        ("imm", 0x4af77b, "c783d00000000000803f"),
        ("imm", 0x4af785, "c743680000803f"),
        ("call", 0x4af78c, 0x20d60),
        ("call", 0x4af794, 0xf590),
        ("rip", 0x4af799, 7, 3, 0x20f0),
        ("rip", 0x4af7a0, 7, 3, 0x1604218),
        ("call", 0x4af7aa, 0xf8f390),
        ("rip", 0x4af7af, 7, 3, 0x15fe518),
        ("call", 0x4af7d2, 0xf8f3a0),
    ],
    "m6_extras": ['bp_atexit_imports', 'bp_object_vtables'],
    "native": {'header': 'native/core/ctor_slot_450_4af4b0.hpp', 'source': 'native/core/ctor_slot_450_4af4b0.cpp', 'test': 'native/tests/test_ctor_slot_450.cpp', 'consts': {'kSlot450Rva': 4912304, 'kSlot450Size': 808, 'kSlot450V0NameLength': 25, 'kSlot450V0DescriptionLength': 0, 'kSlot450V0FnTag': 22808048, 'kSlot450V0Value': 1065353216, 'kSlot450V0Rva': 49258872, 'kSlot450V1NameLength': 17, 'kSlot450V1DescriptionLength': 0, 'kSlot450V1FnTag': 22808048, 'kSlot450V1Value': 1069547520, 'kSlot450V1Rva': 49259088, 'kSlot450V2NameLength': 20, 'kSlot450V2DescriptionLength': 0, 'kSlot450V2FnTag': 22808048, 'kSlot450V2Value': 1065353216, 'kSlot450V2Rva': 49259304},
        'header_tokens': ['IDENTITY TAGS', 'kSlot450V0Name[] = "uiLogoFadeInFromWhiteTime"', 'kSlot450V1Name[] = "uiLogoFadeOutTime"', 'kSlot450V2Name[] = "uiMainMenuStartDelay"'], 'source_order': ['out.v0 = helper_init_unbounded_float_var_site(objects.v0, spec0, concmd);', 'out.v1 = helper_init_unbounded_float_var_site(objects.v1, spec1, concmd);', 'out.v2 = helper_init_unbounded_float_var_site(objects.v2, spec2, concmd);'], 'test_tokens': ['kSlot450Rva == 0x4af4b0 && kSlot450Size == 808', 'section_alone()', 'section_failures()', 'r.refusal == HelperRefusal::none', 'HelperRefusal::unadmitted_arg_functor', 'check_atexit(creg, 0, 0x20f0, &o.v0)']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'three unbounded float console variables (UI logo / main-menu timings)'))
