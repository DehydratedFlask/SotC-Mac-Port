#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 420 (RVA 0x56f6e0, 1006 bytes), batch 6 debug page.

Bool console variable `sndx_debugOcclusionWork` (static/literal shape, value = default = 0, tag 0x15caf18, object 0x2f11408), RANGED int variable `sndx_occlusionWorkIndex` (0xc580, range 0..0x1f,
flags 1, value = default = 0, caller functor tag 0x15caf38, object 0x2f114d0), a 0x26b60 separator (item 0x2f115a8, parent `Sound`, flags 0x30), a 0x26ea0 toggle for the bool (item 0x2f11650,
caption `Show Occlusion Tests`), a 0x27050 slider for the int variable (item 0x2f116f8, caption `Occlusion Index`, range {1.0, 1.0} read from .rodata 0x101e978) and an 8 byte zero cell (0x2f117a0)
with __cxa_atexit(0x56ef80, cell). 0x56ef80 frees a non-null pointer through 0x170b0, so a zero cell is inert; it is an IDENTITY TAG (recorded, not run).

Callee ties: 0x27050 and the 0x26b60 separator constructor are hash-pinned here (the native bodies are tied by check_ctor_slot_361 / check_fctor_slots_298_299_301 / check_fmenu_ctors_slot306). The int
caller functor tag 0x15caf38 is one of the 37 int-family class tables; the native slot reaches 0xc580 through helper_c580_ranged_int and requires HelperRefusal::none.

HOOK: the separator text "MenuSeparator" is built by the native 0xf1a0 through the NOT lifted allocator 0x12820 / 0x12af0; the native slot uses the documented malloc/free stand-in
(default_slot420_hooks) and fails closed when it is cleared. What reads the variables at run time is NOT established.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test;
--tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 420,
    "base": 0x56f6e0,
    "size": 1006,
    "body_sha256": "6e93305bf02b5b0dfda1709199a8e8dcc81ff584c54ef54255709f9ec71ca485",
    "pad": "9090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_ctor', 'menu_item_dtor'],
    "bool_tags": [0x15caf18],
    "float_tags": [],
    "m6_int_tags": [0x15caf38],
    "strings": [
        (0x101e9c0, b'sndx_debugOcclusionWork'),
        (0x101e9d8, b'sndx_occlusionWorkIndex'),
        (0x101e9f0, b'Sound'),
        (0x101e9f6, b'Show Occlusion Tests'),
        (0x101ea0b, b'Occlusion Index'),
        (0x101eaf6, b''),
    ],
    "lengths": [
        ('sndx_debugOcclusionWork', 0x56f748, "48c744244817000000"),
        ('sndx_occlusionWorkIndex', 0x56f862, "48c744242817000000"),
        ('Sound', 0x56f940, "48c744246805000000"),
        ('Sound', 0x56f99b, "48c744246805000000"),
        ('Show Occlusion Tests', 0x56f9b2, "48c744244814000000"),
        ('Sound', 0x56fa24, "48c744246805000000"),
        ('Occlusion Index', 0x56fa2d, "48c74424480f000000"),
    ],
    "pins": [
        ("rip", 0x56f6f8, 7, 3, 0x15fe518),
        ("rip", 0x56f6ff, 7, 3, 0x1594208),
        ("rip", 0x56f706, 7, 3, 0x2f11408),
        ("rip", 0x56f712, 7, 3, 0x101e9c0),
        ("rip", 0x56f71d, 7, 3, 0x101eaf6),
        ("imm", 0x56f724, "b901000000"),
        ("imm", 0x56f72f, "4883c310"),
        ("imm", 0x56f748, "48c744244817000000"),
        ("imm", 0x56f765, "48c744247800000000"),
        ("call", 0x56f771, 0x20af0),
        ("rip", 0x56f776, 7, 3, 0x15940a0),
        ("rip", 0x56f78b, 7, 3, 0x15caf18),
        ("imm", 0x56f792, "4883c010"),
        ("imm", 0x56f79d, "41c7476900000000"),
        ("imm", 0x56f7ab, "49c787a000000000000000"),
        ("imm", 0x56f7bd, "49c787b800000000000000"),
        ("call", 0x56f7c8, 0xf590),
        ("imm", 0x56f7d3, "41c687c000000000"),
        ("imm", 0x56f7db, "41c6476800"),
        ("call", 0x56f7e0, 0x20d60),
        ("call", 0x56f7e8, 0xf590),
        ("rip", 0x56f7ed, 7, 3, 0x20b0),
        ("rip", 0x56f7f4, 7, 3, 0x1604218),
        ("call", 0x56f7fe, 0xf8f390),
        ("rip", 0x56f803, 7, 3, 0x1594bf8),
        ("rip", 0x56f80a, 7, 3, 0x2f114d0),
        ("rip", 0x56f811, 7, 3, 0x101e9d8),
        ("imm", 0x56f82a, "48c784249000000000000000"),
        ("imm", 0x56f836, "c744241c00000000"),
        ("imm", 0x56f83e, "c74424181f000000"),
        ("imm", 0x56f850, "41b901000000"),
        ("rip", 0x56f85b, 7, 3, 0x15caf38),
        ("imm", 0x56f862, "48c744242817000000"),
        ("imm", 0x56f87d, "48c744245800000000"),
        ("imm", 0x56f891, "48c78424a800000000000000"),
        ("imm", 0x56f89d, "4983c510"),
        ("call", 0x56f8b6, 0xc580),
        ("call", 0x56f8c2, 0xf590),
        ("imm", 0x56f8d7, "ff5018"),
        ("imm", 0x56f8e0, "c783d000000000000000"),
        ("imm", 0x56f8ea, "c7436800000000"),
        ("call", 0x56f8f1, 0x20d60),
        ("call", 0x56f8f9, 0xf590),
        ("rip", 0x56f901, 7, 3, 0x1604218),
        ("rip", 0x56f908, 7, 3, 0x18a60),
        ("call", 0x56f912, 0xf8f390),
        ("rip", 0x56f917, 7, 3, 0x2f115a8),
        ("rip", 0x56f923, 7, 3, 0x101e9f0),
        ("imm", 0x56f930, "b930000000"),
        ("imm", 0x56f940, "48c744246805000000"),
        ("call", 0x56f952, 0x26b60),
        ("call", 0x56f95a, 0xf590),
        ("rip", 0x56f95f, 7, 3, 0x272d0),
        ("call", 0x56f96c, 0xf8f390),
        ("rip", 0x56f976, 7, 3, 0x2f11650),
        ("rip", 0x56f987, 7, 3, 0x101e9f6),
        ("imm", 0x56f992, "41b830000000"),
        ("imm", 0x56f99b, "48c744246805000000"),
        ("imm", 0x56f9b2, "48c744244814000000"),
        ("call", 0x56f9c7, 0x26ea0),
        ("call", 0x56f9cf, 0xf590),
        ("call", 0x56f9d7, 0xf590),
        ("rip", 0x56f9dc, 7, 3, 0x272d0),
        ("call", 0x56f9ec, 0xf8f390),
        ("rip", 0x56f9f1, 7, 3, 0x101e9f0),
        ("rip", 0x56f9f8, 7, 3, 0x101ea0b),
        ("rip", 0x56fa03, 7, 3, 0x2f116f8),
        ("imm", 0x56fa14, "41b830000000"),
        ("imm", 0x56fa24, "48c744246805000000"),
        ("imm", 0x56fa2d, "48c74424480f000000"),
        ("rip", 0x56fa42, 8, 4, 0x101e978),
        ("rip", 0x56fa4a, 7, 3, 0x2f114d0),
        ("call", 0x56fa5e, 0x27050),
        ("call", 0x56fa66, 0xf590),
        ("call", 0x56fa6e, 0xf590),
        ("rip", 0x56fa73, 7, 3, 0x272d0),
        ("call", 0x56fa83, 0xf8f390),
        ("rip", 0x56fa88, 7, 3, 0x2f117a0),
        ("rip", 0x56fa8f, 7, 3, 0x56ef80),
        ("imm", 0x56fa99, "48c70600000000"),
        ("call", 0x56faa0, 0xf8f390),
        ("rip", 0x56faa5, 7, 3, 0x15fe518),
        ("call", 0x56fac8, 0xf8f3a0),
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
        'menu_float_ctor_27050': (0x27050, 208, "ad7ca05e3df035fa5df336f0463fb63e331566f140d6f0bf3ad16e9e2766b053"),
        'menu_separator_ctor_26b60': (0x26b60, 166, "7e0c8b7101e14f02fee4e6c32d4315b2ba2a75e146a922fe9ffac79a97a83629"),
        'holder_dtor_56ef80': (0x56ef80, 36, "5df2ac2a78413e96eff32bf0dc31946f14277f9466f13a9f5bbc23e986b2589f"),
    },
    "m6_bytes": [
        (0x270aa, "c7434802000000", '0x27050 kind = 2'),
        (0x2711b, "e990f7ffff", '0x27050 tail jmp 0x268b0'),
        (0x101e978, "0000803f", 'slider min = max = 1.0f (.rodata)'),
        (0xf8f390, "ff25220c6700", '__cxa_atexit PLT thunk'),
    ],
    "m6_extras": ['bp_atexit_imports', 'bp_object_vtables', 'int_site'],
    "native": {'header': 'native/core/ctor_slot_420_56f6e0.hpp', 'source': 'native/core/ctor_slot_420_56f6e0.cpp', 'test': 'native/tests/test_ctor_slot_420.cpp', 'consts': {'kSlot420Rva': 5699296, 'kSlot420Size': 1006, 'kSlot420BoolFnTag': 22851352, 'kSlot420IntFnTag': 22851384, 'kSlot420HolderDtorRva': 5697408, 'kSlot420Var0NameLength': 23, 'kSlot420Var1NameLength': 23, 'kSlot420Var1Min': 0, 'kSlot420Var1Max': 31, 'kSlot420ParentLength': 5, 'kSlot420Item1TextLength': 20, 'kSlot420Item2TextLength': 15, 'kSlot420Item2MinBits': 1065353216, 'kSlot420Item2MaxBits': 1065353216, 'kSlot420ItemFlags': 48, 'kSlot420Var0Rva': 49353736, 'kSlot420Var1Rva': 49353936, 'kSlot420SepRva': 49354152, 'kSlot420Item1Rva': 49354320, 'kSlot420Item2Rva': 49354488, 'kSlot420HolderRva': 49354656},
        'header_tokens': ['kSlot420Var0Name[] = "sndx_debugOcclusionWork"', 'kSlot420Var1Name[] = "sndx_occlusionWorkIndex"', 'kSlot420Parent[] = "Sound"', 'kSlot420Item1Text[] = "Show Occlusion Tests"', 'kSlot420Item2Text[] = "Occlusion Index"', 'IDENTITY TAGS'], 'source_order': ['lane_m6::init_bool_var_literal(objects.var0,', 'helper_init_ranged_int_var_site(objects.var1, spec, concmd)', 'init_menu_separator_26b60(objects.sep, parent, kSlot420ItemFlags, hooks.separator_alloc)', 'register_menu_item_268b0(objects.sep, menu, concmd.atexit_log)', 'record_menu_item_dtor_272d0(concmd.atexit_log, objects.sep)', 'lane_m6::init_toggle_site_target(objects.item1,', 'init_debug_menu_item_27050(objects.item2, label, text, &objects.var1,', 'concmd.atexit_log.record(kDebugMenuItemDtorRva, &objects.item2)', 'concmd.atexit_log.record(kSlot420HolderDtorRva, &objects.holder)'], 'test_tokens': ['kSlot420Rva == 0x56f6e0 && kSlot420Size == 1006', 'section_alone()', 'section_refusal()', 'section_failures()', 'CHECK(creg.atexit_log.size() == 7);', 'CHECK(r.ok && r.refusal == HelperRefusal::none);', 'HelperRefusal::unadmitted_arg_functor', 'Slot420Hooks none{};']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'bool var + ranged int var (0xc580, 0..0x1f, tag 0x15caf38) + 0x26b60 separator + 0x26ea0 toggle + 0x27050 slider + 8 B cell (atexit 0x56ef80)'))
