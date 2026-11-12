#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 430 (RVA 0x529df0, 539 bytes), batch 6.

A zeroed static cell (0x2efc9e0) with __cxa_atexit(0x523ba0), the ranged INT console variable `wanderForceCorruptionLevel` (0xc580, min -1, max 5, flags 1, value = default = -1,
caller functor tag 0x15c8450, atexit 0x18a60) and the debug-menu toggle `Shadow Cheat` / `Wander Corrupt Level` targeting it (0x26ea0, flags 0x30, atexit 0x272d0).

Helper-callee tie: the caller functor tag 0x15c8450 is one of the 37 int-family class tables (slots {0xc720, 0xc740, 0xc760, 0xc770}); the native slot reaches
0xc580 through helper_c580_ranged_int and requires HelperRefusal::none. tools/lift/check_helper_c580.py pins the same call site (lea 0x529ec8, call 0x529f02).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. Pins: constructor-table word, whole-body sha256 and
extent, every rip-relative operand, call target, immediate and indirect call of the body, the referenced strings and length immediates, the functor vtable of the caller
tag and the hash of every shared callee. The inherited driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 430,
    "base": 0x529df0,
    "size": 539,
    "body_sha256": "ec8738216cc8bc929435eb74f01808fb04a13042fa7478d5a4f94f6686a3c93d",
    "pad": "9090909090",
    "callees": ['concmd_register', 'release_f590', 'menu_toggle_ctor', 'menu_item_dtor'],
    "bool_tags": [],
    "float_tags": [],
    "m6_int_tags": [0x15c8450],
    "strings": [
        (0x10131e3, b'wanderForceCorruptionLevel'),
        (0x10131fe, b'Set Corrupt Unlock.'),
        (0x1013212, b'Shadow Cheat'),
        (0x101321f, b'Wander Corrupt Level'),
    ],
    "lengths": [
        ('wanderForceCorruptionLevel', 0x529e9e, "48c74424481a000000"),
        ('Set Corrupt Unlock.', 0x529eb5, "48c744242013000000"),
        ('Shadow Cheat', 0x529f9d, "48c74424680c000000"),
        ('Wander Corrupt Level', 0x529fa6, "48c744244814000000"),
    ],
    "pins": [
        ("rip", 0x529e08, 7, 3, 0x15fe518),
        ("rip", 0x529e0f, 7, 3, 0x2efc9e0),
        ("rip", 0x529e16, 7, 3, 0x523ba0),
        ("rip", 0x529e1d, 7, 3, 0x1604218),
        ("imm", 0x529e2f, "48c70600000000"),
        ("call", 0x529e36, 0xf8f390),
        ("rip", 0x529e3b, 7, 3, 0x1594bf8),
        ("rip", 0x529e42, 7, 3, 0x2efc9e8),
        ("rip", 0x529e49, 7, 3, 0x10131e3),
        ("imm", 0x529e59, "c744243cffffffff"),
        ("imm", 0x529e61, "c744243805000000"),
        ("imm", 0x529e71, "48c784249000000000000000"),
        ("imm", 0x529e8c, "41b901000000"),
        ("rip", 0x529e97, 7, 3, 0x10131fe),
        ("imm", 0x529e9e, "48c74424481a000000"),
        ("imm", 0x529eb5, "48c744242013000000"),
        ("rip", 0x529ec8, 7, 3, 0x15c8450),
        ("imm", 0x529edd, "48c78424a800000000000000"),
        ("imm", 0x529eee, "4983c610"),
        ("call", 0x529f02, 0xc580),
        ("imm", 0x529f16, "ff5018"),
        ("imm", 0x529f1f, "c783d0000000ffffffff"),
        ("imm", 0x529f29, "c74368ffffffff"),
        ("call", 0x529f30, 0x20d60),
        ("call", 0x529f38, 0xf590),
        ("call", 0x529f40, 0xf590),
        ("rip", 0x529f45, 7, 3, 0x1604218),
        ("rip", 0x529f4c, 7, 3, 0x18a60),
        ("call", 0x529f59, 0xf8f390),
        ("rip", 0x529f5e, 7, 3, 0x101321f),
        ("rip", 0x529f65, 7, 3, 0x2efcac0),
        ("rip", 0x529f76, 7, 3, 0x1013212),
        ("imm", 0x529f81, "41b830000000"),
        ("imm", 0x529f9d, "48c74424680c000000"),
        ("imm", 0x529fa6, "48c744244814000000"),
        ("call", 0x529fbb, 0x26ea0),
        ("call", 0x529fc3, 0xf590),
        ("call", 0x529fcb, 0xf590),
        ("rip", 0x529fd0, 7, 3, 0x272d0),
        ("call", 0x529fdd, 0xf8f390),
        ("rip", 0x529fe2, 7, 3, 0x15fe518),
        ("call", 0x52a005, 0xf8f3a0),
    ],
    "m6_callees": {
        'ranged_int_ctor_c580': (0xc580, 351, "a93bc0f34cfd49fbbcbc7f2186b2668de6e49f5f54012caafca7f17dfb396bce"),
        'int_fn_copy_c720': (0xc720, 20, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
        'int_fn_clone_c740': (0xc740, 20, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
        'int_fn_move_c760': (0xc760, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
        'int_fn_destroy_c770': (0xc770, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
        'int_dtor_18a60': (0x18a60, 54, "4de4f3993b02c84c9c20824690e8ab9d4f7a1b84babfa6cbe0d89c9dd558b50b"),
        'int_slot3_bf70': (0xbf70, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
        'int_slot6_bfc0': (0xbfc0, 6, "7140f35dee6220b79b12aecc27acf5105bf3b77d1588e89fce345de7c16c72b7"),
        'cell_dtor_523ba0': (0x523ba0, 46, "551a3a8048d18984ecbd08756e2be011f1362b4a5b6390326b1fb3bb5b7507f1"),
    },
    "m6_extras": ['bp_atexit_imports', 'int_site', 'toggle_gate'],
    "native": {'header': 'native/core/ctor_slot_430_529df0.hpp', 'source': 'native/core/ctor_slot_430_529df0.cpp', 'test': 'native/tests/test_ctor_slot_430.cpp', 'shapes_h': 'native/core/helper_int_var_sites.hpp', 'shapes_c': 'native/core/helper_int_var_sites.cpp', 'consts': {'kSlot430Rva': 5414384, 'kSlot430Size': 539, 'kSlot430IntFnTag': 22840400, 'kSlot430CellDtorRva': 5389216, 'kSlot430VarNameLength': 26, 'kSlot430VarDescriptionLength': 19, 'kSlot430Max': 5, 'kSlot430CellRva': 49269216, 'kSlot430VarRva': 49269224, 'kSlot430ItemRva': 49269440},
        'header_tokens': ['kSlot430Min = -1', 'kSlot430Value = -1', 'kSlot430VarName[] = "wanderForceCorruptionLevel"', 'kSlot430VarDescription[] = "Set Corrupt Unlock."', 'kSlot430MenuLabel[] = "Shadow Cheat"', 'kSlot430MenuText[] = "Wander Corrupt Level"', 'IDENTITY TAGS'], 'source_order': ['std::atomic_ref<std::uint64_t>(objects.cell).store(0, std::memory_order_seq_cst)', 'concmd.atexit_log.record(kSlot430CellDtorRva, &objects.cell)', 'helper_init_ranged_int_var_site(objects.var, spec, concmd)', 'lane_m6::init_toggle_site_target(objects.item, ranged_int_c580_menu_target(objects.var),'], 'test_tokens': ['kSlot430Rva == 0x529df0 && kSlot430Size == 539', 'section_alone()', 'section_tag_refusal()', 'section_failures()', 'check_atexit(creg, 0, 0x523ba0, &o.cell)', 'r.var.refusal == HelperRefusal::none', 'HelperRefusal::unadmitted_arg_functor'], 'shapes_tokens': ['helper_c580_ranged_int(var, name, &var.value, spec.min_value, spec.max_value,', 'out.functor_destroy_ok = ctor.arg_destroy_ok;', 'registry.atexit_log.record(kRangedIntDtorRva, &var)', 'register_concmd_20d60(var.base, registry)']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'ranged int var wanderForceCorruptionLevel (-1..5, tag 0x15c8450 via the c580 family wrapper) + cell atexit 0x523ba0 + debug-menu toggle'))
