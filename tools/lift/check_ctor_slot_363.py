#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 363 (RVA 0x751c70, 701 bytes), batch 11 debug page (menu-less cp3_animal* variables).

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The page has NO menu rows. The local ranged-int variables go through the wrapper family (byte twins of 0x7ec160, family proven in
check_helper_620e10.py); the lint_sites extra ties EACH site's caller tag to the wrapper's own functor class (+0x5a) and its atexit destructor to slot 0 of the wrapper's final class (per-site tag proof, never inherited from another
slot). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (the readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

import local_int_site_lib  # noqa: E402,F401

SPEC = {
    "slot": 363,
    "base": 0x751c70,
    "size": 701,
    "body_sha256": "bff149d0b27fd9bdbd2f5f87f7a2eb834ac5ce235e38c1e587f0170b4d411451",
    "pad": "909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'release_f590'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0x127fe20, b'cp3_animalDebugType'),
        (0x127fe34, b'cp3_animalDisplayType'),
        (0x127fe4a, b'cp3_animalDebugID'),
    ],
    "lengths": [
        ('cp3_animalDebugType', 0x751caf, "48c744242813000000"),
        ('cp3_animalDisplayType', 0x751da6, "48c744242815000000"),
        ('cp3_animalDebugID', 0x751e70, "48c744242811000000"),
    ],
    "pins": [
        ("rip", 0x751c88, 7, 3, 0x15fe518),
        ("rip", 0x751c8f, 7, 3, 0x127fe20),
        ("imm", 0x751caf, "48c744242813000000"),
        ("rip", 0x751cc2, 7, 3, 0x15cd3c8),
        ("imm", 0x751cc9, "48c744247000000000"),
        ("imm", 0x751cd2, "c744241c00000000"),
        ("imm", 0x751cda, "c744241800000000"),
        ("imm", 0x751ce2, "c744241406000000"),
        ("imm", 0x751cf8, "48c784248800000000000000"),
        ("call", 0x751d04, 0xf730),
        ("rip", 0x751d09, 7, 3, 0x3252730),
        ("imm", 0x751d29, "41b901000000"),
        ("call", 0x751d38, 0x751190),
        ("imm", 0x751d4c, "ff5018"),
        ("call", 0x751d52, 0xf590),
        ("rip", 0x751d57, 7, 3, 0x1604218),
        ("rip", 0x751d5e, 7, 3, 0x74aae0),
        ("call", 0x751d6b, 0xf8f390),
        ("rip", 0x751d70, 7, 3, 0x127fe34),
        ("imm", 0x751d7b, "c744241c00000000"),
        ("imm", 0x751d83, "c744241800000000"),
        ("imm", 0x751d8b, "c744241409000000"),
        ("imm", 0x751d98, "48c744247000000000"),
        ("imm", 0x751da6, "48c744242815000000"),
        ("rip", 0x751db9, 7, 3, 0x15cd460),
        ("imm", 0x751dce, "48c784248800000000000000"),
        ("call", 0x751dda, 0xf730),
        ("rip", 0x751ddf, 7, 3, 0x3252808),
        ("imm", 0x751dfa, "41b901000000"),
        ("call", 0x751e09, 0x751550),
        ("imm", 0x751e1d, "ff5018"),
        ("call", 0x751e23, 0xf590),
        ("rip", 0x751e28, 7, 3, 0x74ab20),
        ("call", 0x751e35, 0xf8f390),
        ("rip", 0x751e3a, 7, 3, 0x127fe4a),
        ("imm", 0x751e45, "c744241c14000000"),
        ("imm", 0x751e4d, "c744241800000000"),
        ("imm", 0x751e55, "c744241414000000"),
        ("imm", 0x751e62, "48c744247000000000"),
        ("imm", 0x751e70, "48c744242811000000"),
        ("rip", 0x751e83, 7, 3, 0x15cd4f8),
        ("imm", 0x751e98, "48c784248800000000000000"),
        ("call", 0x751ea4, 0xf730),
        ("rip", 0x751ea9, 7, 3, 0x32528e0),
        ("imm", 0x751ec4, "41b901000000"),
        ("call", 0x751ed3, 0x751910),
        ("imm", 0x751ee7, "ff5018"),
        ("call", 0x751eed, 0xf590),
        ("rip", 0x751ef2, 7, 3, 0x74ab60),
        ("call", 0x751eff, 0xf8f390),
        ("rip", 0x751f04, 7, 3, 0x15fe518),
        ("call", 0x751f27, 0xf8f3a0),
    ],
    "m6_callees": {
        'wrapper_template_7ec160': (0x7ec160, 435, "30b38c418e5682d6024b6b14aae540471882733b02d715752a867ba753f25307"),
        'ranged_int_ctor_c580': (0xc580, 351, "a93bc0f34cfd49fbbcbc7f2186b2668de6e49f5f54012caafca7f17dfb396bce"),
        'int_fn_copy_c720': (0xc720, 20, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
        'int_fn_clone_c740': (0xc740, 20, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
        'int_fn_move_c760': (0xc760, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
        'int_fn_destroy_c770': (0xc770, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
        'wrapper_751190': (0x751190, 435, "661546b21db00bdbfee7589da586c5b48ed74d96927dab38d303396c7aca7920"),
        'inner_751350': (0x751350, 351, "b415dac5a25fc1e179efccbb64f1f559bc1adb36a22c664ad20d43aa3e0e4289"),
        'dtor_74aae0': (0x74aae0, 54, "78a16939956710bfa39ef950e2fe6514d914874c3d517825cefb27d03f9df725"),
        'wrapper_751550': (0x751550, 435, "6922c7cb44db76ee7ed8993eb5098986c25712061985612db916ec2e0d32e16a"),
        'inner_751710': (0x751710, 351, "b5de50aa1e2c821a1631f603ef32aad40b8580e9d7be3aaaa4edb59d480ac216"),
        'dtor_74ab20': (0x74ab20, 54, "f7fa090bdafa19969e759df9248d65b8dcf072a9baae77c7a562a0b5524a3219"),
        'wrapper_751910': (0x751910, 435, "6c679e4ca2fb8f86f219bbe2bd75b8aa5655ba47d1db69318729807d1c786f52"),
        'inner_751ad0': (0x751ad0, 351, "d8379aad567433b4f8f847ed6f1b7aefce7cc9dc89878736ba18f998c3190c3f"),
        'dtor_74ab60': (0x74ab60, 54, "e3d3ac378d10588f55888436eb6fa774da8230c4d75760e910fb5f6c85d51e07"),
    },
    'lint_sites': [{'wrapper': 7672208, 'own': 22860744, 'call': 7675192, 'ind': 7675212, 'atexit_call': 7675243, 'f730': 7675140, 'tag': 22860744, 'tag_lea': 7675074, 'dtor': 7645920, 'dtor_lea': 7675230, 'obj': 52766512, 'obj_lea': 7675145}, {'wrapper': 7673168, 'own': 22860896, 'call': 7675401, 'ind': 7675421, 'atexit_call': 7675445, 'f730': 7675354, 'tag': 22860896, 'tag_lea': 7675321, 'dtor': 7645984, 'dtor_lea': 7675432, 'obj': 52766728, 'obj_lea': 7675359}, {'wrapper': 7674128, 'own': 22861048, 'call': 7675603, 'ind': 7675623, 'atexit_call': 7675647, 'f730': 7675556, 'tag': 22861048, 'tag_lea': 7675523, 'dtor': 7646048, 'dtor_lea': 7675634, 'obj': 52766944, 'obj_lea': 7675561}],
    "m6_extras": ['bp_atexit_imports', 'lint_sites'],
    "native": {'header': 'native/core/ctor_slot_363_751c70.hpp', 'source': 'native/core/ctor_slot_363_751c70.cpp', 'test': 'native/tests/test_ctor_slot_363.cpp', 'consts': {'kSlot363Rva': 7674992, 'kSlot363Size': 701, 'kSlot363BoolFnTag': 0, 'kSlot363FloatFnTag': 0, 'kSlot363ItemFlags': 48, 'kSlot363VarFlags': 1, 'kSlot363BoolCount': 0, 'kSlot363FloatCount': 0, 'kSlot363UFloatFnTag': 0, 'kSlot363UFloatCount': 0, 'kSlot363IntFnTag': 0, 'kSlot363IntCount': 0, 'kSlot363LocalIntCount': 3, 'kSlot363RowCount': 3, 'kSlot363StringCount': 0, 'kSlot363SliderCount': 0, 'kSlot363Object0Rva': 52766512, 'kSlot363CellCount': 0, 'kSlot363HolderCount': 0},
        'header_tokens': ['"cp3_animalDebugType"', '"cp3_animalDisplayType"', '"cp3_animalDebugID"', 'IDENTITY TAGS'], 'source_order': ['helper_init_local_int_var_site(var, kSlot363LocalIntSites[n], empty_string_f730(), concmd)', 'objects.r00', 'objects.r01', 'objects.r02'], 'test_tokens': ['kSlot363Rva == 0x751c70 && kSlot363Size == 701', 'section_constants()', 'section_alone()', 'section_failures()', 'CHECK(items == 0);', 'creg.atexit_log.size() == kSlot363RowCount - cells);', 'r.rows_done == 0']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'three local-int wrapper sites (0x751190 / 0x751550 / 0x751910), menu-less'))
