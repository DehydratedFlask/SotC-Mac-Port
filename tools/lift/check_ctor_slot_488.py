#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 488 (RVA 0x2ba5d0, 1028 bytes), batch 6.

RANGED int console variable `SM_showActiveState` (0xc580, range 0..2, flags 1, value = default = 0, empty description, caller functor tag 0x15a0a18, object 0x2eef788), then a 0x30 byte
static holder built inline (object 0x2eef860: class pointer 0x15a0a38 + 0x10, borrowed view `SM_debugIdentifier`, 0x18 zero bytes, f590 of the temporary view, atexit 0x2b5050), then the
UNBOUNDED int variables `SM_showActiveStateMinTime` and `SM_showActiveStateMaxTime` (0x3a980, flags 1, value = default = 0, same functor tag). No menu item.

Helper-callee tie: the caller functor tag 0x15a0a18 is one of the 37 int-family class tables; the native slot reaches 0xc580 / 0x3a980 through helper_c580_ranged_int /
helper_3a980_unbounded_int and requires HelperRefusal::none for all three sites. The 0x2b5050 holder destructor and the 0x15a0a38 class are IDENTITY TAGS only: the
holder body is not claimed beyond the stores pinned here.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test;
--tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 488,
    "base": 0x2ba5d0,
    "size": 1028,
    "body_sha256": "06002a8a1b3274c0f86c34a8f650c792aea33a6f53855a98124fa1ece5bcd83f",
    "pad": "909090909090909090909090",
    "callees": ['concmd_register', 'release_f590'],
    "bool_tags": [],
    "float_tags": [],
    "m6_int_tags": [0x15a0a18],
    "strings": [
        (0xfcc1c0, b'SM_showActiveState'),
        (0xfcc1d3, b'SM_debugIdentifier'),
        (0xfcc1e6, b'SM_showActiveStateMinTime'),
        (0xfcc200, b'SM_showActiveStateMaxTime'),
        (0xfcccad, b''),
    ],
    "lengths": [
        ('SM_showActiveState', 0x2ba651, "48c744242812000000"),
        ('SM_debugIdentifier', 0x2ba754, "48c744246812000000"),
        ('SM_showActiveStateMinTime', 0x2ba824, "48c744242819000000"),
        ('SM_showActiveStateMaxTime', 0x2ba925, "48c744242819000000"),
    ],
    "pins": [
        ("rip", 0x2ba5e8, 7, 3, 0x15fe518),
        ("rip", 0x2ba5ef, 7, 3, 0x1594bf8),
        ("rip", 0x2ba5f6, 7, 3, 0xfcc1c0),
        ("rip", 0x2ba5fd, 7, 3, 0x2eef788),
        ("rip", 0x2ba611, 7, 3, 0x15a0a18),
        ("imm", 0x2ba62a, "41b901000000"),
        ("imm", 0x2ba639, "4983c710"),
        ("rip", 0x2ba64a, 7, 3, 0xfcccad),
        ("imm", 0x2ba651, "48c744242812000000"),
        ("imm", 0x2ba65a, "48c784249000000000000000"),
        ("imm", 0x2ba67a, "c744241c00000000"),
        ("imm", 0x2ba682, "c744241802000000"),
        ("rip", 0x2ba68a, 7, 3, 0x2eef788),
        ("imm", 0x2ba691, "48c78424a800000000000000"),
        ("rip", 0x2ba6a2, 7, 3, 0x2eef858),
        ("imm", 0x2ba6b4, "48c744245800000000"),
        ("call", 0x2ba6c4, 0xc580),
        ("call", 0x2ba6d0, 0xf590),
        ("imm", 0x2ba6e0, "ff5018"),
        ("rip", 0x2ba6e6, 7, 3, 0x2eef788),
        ("rip", 0x2ba6ed, 10, 2, 0x2eef858),
        ("imm", 0x2ba6ed, "c7056151c30200000000"),
        ("rip", 0x2ba6f7, 10, 2, 0x2eef7f0),
        ("imm", 0x2ba6f7, "c705ef50c30200000000"),
        ("call", 0x2ba701, 0x20d60),
        ("call", 0x2ba709, 0xf590),
        ("rip", 0x2ba70e, 7, 3, 0x1604218),
        ("rip", 0x2ba715, 7, 3, 0x18a60),
        ("call", 0x2ba722, 0xf8f390),
        ("rip", 0x2ba727, 7, 3, 0x15a0a38),
        ("rip", 0x2ba72e, 7, 3, 0xfcc1d3),
        ("rip", 0x2ba73e, 7, 3, 0x2eef860),
        ("imm", 0x2ba74f, "b812000000"),
        ("imm", 0x2ba754, "48c744246812000000"),
        ("rip", 0x2ba76c, 8, 4, 0x2eef868),
        ("rip", 0x2ba774, 8, 4, 0x2eef878),
        ("rip", 0x2ba77c, 11, 3, 0x2eef888),
        ("imm", 0x2ba77c, "48c7050151c30200000000"),
        ("imm", 0x2ba787, "4883c110"),
        ("rip", 0x2ba78b, 7, 3, 0x2eef860),
        ("call", 0x2ba792, 0xf590),
        ("rip", 0x2ba797, 7, 3, 0x2b5050),
        ("call", 0x2ba7a4, 0xf8f390),
        ("rip", 0x2ba7a9, 7, 3, 0x2eef890),
        ("rip", 0x2ba7b0, 7, 3, 0xfcccad),
        ("rip", 0x2ba7c0, 7, 3, 0x15a0a18),
        ("rip", 0x2ba7cc, 7, 3, 0xfcc1e6),
        ("imm", 0x2ba7d3, "48c784249000000000000000"),
        ("imm", 0x2ba7e8, "b901000000"),
        ("rip", 0x2ba7ed, 7, 3, 0x2eef890),
        ("rip", 0x2ba80f, 7, 3, 0x2eef960),
        ("imm", 0x2ba824, "48c744242819000000"),
        ("imm", 0x2ba839, "48c78424a800000000000000"),
        ("imm", 0x2ba845, "48c744245800000000"),
        ("call", 0x2ba84e, 0x3a980),
        ("call", 0x2ba856, 0xf590),
        ("imm", 0x2ba866, "ff5018"),
        ("rip", 0x2ba86c, 7, 3, 0x2eef890),
        ("rip", 0x2ba873, 10, 2, 0x2eef960),
        ("imm", 0x2ba873, "c705e350c30200000000"),
        ("rip", 0x2ba87d, 10, 2, 0x2eef8f8),
        ("imm", 0x2ba87d, "c7057150c30200000000"),
        ("call", 0x2ba887, 0x20d60),
        ("call", 0x2ba88f, 0xf590),
        ("rip", 0x2ba894, 7, 3, 0x18a60),
        ("rip", 0x2ba89b, 7, 3, 0x1604218),
        ("call", 0x2ba8a5, 0xf8f390),
        ("rip", 0x2ba8aa, 7, 3, 0x2eef968),
        ("rip", 0x2ba8b1, 7, 3, 0xfcccad),
        ("rip", 0x2ba8c1, 7, 3, 0x15a0a18),
        ("rip", 0x2ba8cd, 7, 3, 0xfcc200),
        ("imm", 0x2ba8d4, "48c784249000000000000000"),
        ("imm", 0x2ba8e9, "b901000000"),
        ("rip", 0x2ba8ee, 7, 3, 0x2eef968),
        ("rip", 0x2ba910, 7, 3, 0x2eefa38),
        ("imm", 0x2ba925, "48c744242819000000"),
        ("imm", 0x2ba93a, "48c78424a800000000000000"),
        ("imm", 0x2ba946, "48c744245800000000"),
        ("call", 0x2ba94f, 0x3a980),
        ("call", 0x2ba957, 0xf590),
        ("imm", 0x2ba967, "ff5018"),
        ("rip", 0x2ba96d, 7, 3, 0x2eef968),
        ("rip", 0x2ba974, 10, 2, 0x2eefa38),
        ("imm", 0x2ba974, "c705ba50c30200000000"),
        ("rip", 0x2ba97e, 10, 2, 0x2eef9d0),
        ("imm", 0x2ba97e, "c7054850c30200000000"),
        ("call", 0x2ba988, 0x20d60),
        ("call", 0x2ba990, 0xf590),
        ("rip", 0x2ba995, 7, 3, 0x18a60),
        ("rip", 0x2ba99c, 7, 3, 0x1604218),
        ("call", 0x2ba9a6, 0xf8f390),
        ("rip", 0x2ba9ab, 7, 3, 0x15fe518),
        ("call", 0x2ba9ce, 0xf8f3a0),
    ],
    "m6_callees": {
        'ranged_int_ctor_c580': (0xc580, 351, "a93bc0f34cfd49fbbcbc7f2186b2668de6e49f5f54012caafca7f17dfb396bce"),
        'int_var_worker_3a980': (0x3a980, 322, "2cbfc2763abb6506a59551b902dd6098325889e5e520d842c0e93da7f43498ca"),
        'int_fn_copy_c720': (0xc720, 20, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
        'int_fn_clone_c740': (0xc740, 20, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
        'int_fn_move_c760': (0xc760, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
        'int_fn_destroy_c770': (0xc770, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
        'int_dtor_18a60': (0x18a60, 54, "4de4f3993b02c84c9c20824690e8ab9d4f7a1b84babfa6cbe0d89c9dd558b50b"),
        'int_slot3_bf70': (0xbf70, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
        'int_slot6_bfc0': (0xbfc0, 6, "7140f35dee6220b79b12aecc27acf5105bf3b77d1588e89fce345de7c16c72b7"),
    },
    "m6_extras": ['bp_atexit_imports', 'int_site'],
    "native": {'header': 'native/core/ctor_slot_488_2ba5d0.hpp', 'source': 'native/core/ctor_slot_488_2ba5d0.cpp', 'test': 'native/tests/test_ctor_slot_488.cpp', 'consts': {'kSlot488Rva': 2860496, 'kSlot488Size': 1028, 'kSlot488IntFnTag': 22678040, 'kSlot488HolderVtbl': 22678088, 'kSlot488HolderDtorRva': 2838608, 'kSlot488Var0NameLength': 18, 'kSlot488HolderTextLength': 18, 'kSlot488Var1NameLength': 25, 'kSlot488Var2NameLength': 25, 'kSlot488Var0Min': 0, 'kSlot488Var0Max': 2, 'kSlot488Var0Rva': 49215368, 'kSlot488HolderRva': 49215584, 'kSlot488Var1Rva': 49215632, 'kSlot488Var2Rva': 49215848},
        'header_tokens': ['kSlot488Var0Name[] = "SM_showActiveState"', 'kSlot488HolderText[] = "SM_debugIdentifier"', 'kSlot488Var1Name[] = "SM_showActiveStateMinTime"', 'kSlot488Var2Name[] = "SM_showActiveStateMaxTime"', 'IDENTITY TAGS'], 'source_order': ['helper_init_ranged_int_var_site(objects.var0, spec0, concmd)', 'objects.holder.vptr = kSlot488HolderVtbl', 'concmd.atexit_log.record(kSlot488HolderDtorRva, &objects.holder)', 'helper_init_unbounded_int_var_site(objects.var1, spec1, concmd)', 'helper_init_unbounded_int_var_site(objects.var2, spec2, concmd)'], 'test_tokens': ['kSlot488Rva == 0x2ba5d0 && kSlot488Size == 1028', 'section_alone()', 'section_tag_refusal()', 'section_failures()', 'check_atexit(creg, 1, 0x2b5050, &o.holder)', 's.ok && s.refusal == HelperRefusal::none', 'check_site(r.var0)', 'check_site(r.var1)', 'check_site(r.var2)', 'HelperRefusal::unadmitted_arg_functor']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'ranged int SM_showActiveState (0..2, 0xc580) + 0x30 B holder (atexit 0x2b5050) + unbounded ints SM_showActiveStateMinTime / MaxTime (0x3a980), tag 0x15a0a18'))
