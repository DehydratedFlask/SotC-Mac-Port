#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 339 (RVA 0x7e2fc0, 1004 bytes), batch 9 (the startup-table cliff of batch 8).

Seven `{name pointer, 0x52a990(name)}` hash cells (0x325ffb0 + 0x10 * n; 0x52a990 is the native fnv_cstr_52a990 = `mov esi,0x811c9dc5; jmp 0x161fb0`, so the batch-8 "unmodelled caller" flag was a naming miss), two 0x30 byte name
holders (objects 0x3260020 / 0x3260050: class pointer 0x15976a0 + 0x10, borrowed views `shadowDemoMode` / `shadowDemoMode_gameplayIntro`, zero tail, atexit 0x8ef00), one UNBOUNDED float variable
`shadowDemoMode_gameplayTimer` (0x6aa00, flags 1, value = default 120.0f, caller functor tag 0x15ce440, object 0x3260080, atexit 0x20f0) and the console command `killBoss` (object 0x3260160, flags 2, handler tag 0x7def00 never
called, atexit 0x4b0). The batch-8 "non-functor indirect call" `callq *0x20(%rax)` at 0x7e3356 is the console-command register-once slot [0x1594dc0 + 0x20] = 0x20d60 (the `fcmd_vtable` extra pins that relocation), exactly as in
slots 306 / 343 / 386 / 557; the other indirect call `*0x18(%rax)` is the float functor destroy of the 68-table family.

The holder destructor 0x8ef00 and the handler body 0x7def00 are hash-pinned; vtables, tags and destructors are IDENTITY TAGS (recorded, never run). The demo-mode / play-time / boss reading is inference from the strings.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 339,
    "base": 0x7e2fc0,
    "size": 1004,
    "body_sha256": "144e9f617b22954952ef91e0607cc1586c08ab51d8e3d26cb0ab9cc7e1d0d07b",
    "pad": "90909090",
    "callees": ['concmd_register', 'unbounded_float_ctor', 'float_dtor', 'release_f590', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy', 'concmd_base_ctor', 'cmd_dtor'],
    "bool_tags": [],
    "float_tags": [0x15ce440],
    "strings": [
        (0x128a144, b'CurrentPlayTimeEASY'),
        (0x128a158, b'CurrentPlayTimeNORMAL'),
        (0x128a16e, b'CurrentPlayTimeHARD'),
        (0x128a182, b'FastestPlayTimeEASY'),
        (0x128a196, b'FastestPlayTimeNORMAL'),
        (0x128a1ac, b'FastestPlayTimeHARD'),
        (0x128a1c0, b'CurrentDeathCount'),
        (0x128a1d2, b'shadowDemoMode'),
        (0x128a1e1, b'shadowDemoMode_gameplayIntro'),
        (0x128a1fe, b'shadowDemoMode_gameplayTimer'),
        (0x128a21b, b'killBoss'),
        (0x128a224, b'Kills the current boss if present. Params: None'),
        (0x128a400, b''),
    ],
    "lengths": [
        ('shadowDemoMode', 0x7e30c6, "48c74424480e000000"),
        ('shadowDemoMode_gameplayIntro', 0x7e314b, "48c74424481c000000"),
        ('shadowDemoMode_gameplayTimer', 0x7e31f7, "48c74424081c000000"),
        ('Kills the current boss if present. Params: None', 0x7e32e9, "48c74424282f000000"),
    ],
    "pins": [
        ("rip", 0x7e2fd8, 7, 3, 0x15fe518),
        ("rip", 0x7e2fdf, 7, 3, 0x325ffb0),
        ("rip", 0x7e2fe6, 7, 3, 0x128a144),
        ("call", 0x7e2ffb, 0x52a990),
        ("rip", 0x7e3003, 7, 3, 0x325ffc0),
        ("rip", 0x7e300a, 7, 3, 0x128a158),
        ("call", 0x7e3014, 0x52a990),
        ("rip", 0x7e301c, 7, 3, 0x325ffd0),
        ("rip", 0x7e3023, 7, 3, 0x128a16e),
        ("call", 0x7e302d, 0x52a990),
        ("rip", 0x7e3035, 7, 3, 0x325ffe0),
        ("rip", 0x7e303c, 7, 3, 0x128a182),
        ("call", 0x7e3046, 0x52a990),
        ("rip", 0x7e304e, 7, 3, 0x325fff0),
        ("rip", 0x7e3055, 7, 3, 0x128a196),
        ("call", 0x7e305f, 0x52a990),
        ("rip", 0x7e3067, 7, 3, 0x3260000),
        ("rip", 0x7e306e, 7, 3, 0x128a1ac),
        ("call", 0x7e3078, 0x52a990),
        ("rip", 0x7e3080, 7, 3, 0x3260010),
        ("rip", 0x7e3087, 7, 3, 0x128a1c0),
        ("call", 0x7e3091, 0x52a990),
        ("rip", 0x7e3099, 7, 3, 0x15976a0),
        ("rip", 0x7e30a0, 7, 3, 0x128a1d2),
        ("rip", 0x7e30b0, 7, 3, 0x3260020),
        ("imm", 0x7e30c1, "b80e000000"),
        ("imm", 0x7e30c6, "48c74424480e000000"),
        ("rip", 0x7e30de, 8, 4, 0x3260028),
        ("rip", 0x7e30e6, 8, 4, 0x3260038),
        ("rip", 0x7e30ee, 11, 3, 0x3260048),
        ("imm", 0x7e30ee, "48c7054fcfa70200000000"),
        ("imm", 0x7e30f9, "4883c310"),
        ("rip", 0x7e30fd, 7, 3, 0x3260020),
        ("call", 0x7e3104, 0xf590),
        ("rip", 0x7e3109, 7, 3, 0x8ef00),
        ("rip", 0x7e3110, 7, 3, 0x1604218),
        ("call", 0x7e3120, 0xf8f390),
        ("rip", 0x7e3125, 7, 3, 0x128a1e1),
        ("rip", 0x7e3135, 7, 3, 0x3260050),
        ("imm", 0x7e3146, "b81c000000"),
        ("imm", 0x7e314b, "48c74424481c000000"),
        ("rip", 0x7e3163, 8, 4, 0x3260058),
        ("rip", 0x7e316b, 8, 4, 0x3260068),
        ("rip", 0x7e3173, 11, 3, 0x3260078),
        ("imm", 0x7e3173, "48c705facea70200000000"),
        ("rip", 0x7e317e, 7, 3, 0x3260050),
        ("call", 0x7e3185, 0xf590),
        ("call", 0x7e3193, 0xf8f390),
        ("rip", 0x7e3198, 7, 3, 0x15942a0),
        ("rip", 0x7e319f, 7, 3, 0x128a1fe),
        ("rip", 0x7e31a6, 7, 3, 0x3260080),
        ("imm", 0x7e31ba, "48c744247000000000"),
        ("rip", 0x7e31c7, 7, 3, 0x3260150),
        ("imm", 0x7e31d3, "b901000000"),
        ("rip", 0x7e31e1, 7, 3, 0x15ce440),
        ("imm", 0x7e31f7, "48c74424081c000000"),
        ("rip", 0x7e320e, 7, 3, 0x128a400),
        ("imm", 0x7e3215, "48c784248800000000000000"),
        ("imm", 0x7e322c, "48c744243800000000"),
        ("imm", 0x7e3235, "4983c610"),
        ("rip", 0x7e3239, 7, 3, 0x3260080),
        ("call", 0x7e3240, 0x6aa00),
        ("call", 0x7e3248, 0xf590),
        ("imm", 0x7e3258, "ff5018"),
        ("rip", 0x7e325e, 7, 3, 0x3260080),
        ("imm", 0x7e3265, "c705e1cea7020000f042"),
        ("imm", 0x7e326f, "c7056fcea7020000f042"),
        ("call", 0x7e3279, 0x20d60),
        ("call", 0x7e3281, 0xf590),
        ("rip", 0x7e3286, 7, 3, 0x20f0),
        ("rip", 0x7e328d, 7, 3, 0x1604218),
        ("call", 0x7e3297, 0xf8f390),
        ("rip", 0x7e32a1, 7, 3, 0x128a21b),
        ("call", 0x7e32ad, 0xf2f0),
        ("rip", 0x7e32b2, 7, 3, 0x3260160),
        ("rip", 0x7e32b9, 8, 4, 0x15fe5e0),
        ("rip", 0x7e32c1, 7, 3, 0x128a224),
        ("imm", 0x7e32d1, "b902000000"),
        ("rip", 0x7e32e2, 7, 3, 0x7def00),
        ("imm", 0x7e32e9, "48c74424282f000000"),
        ("call", 0x7e3306, 0x20af0),
        ("rip", 0x7e330b, 7, 3, 0x1594dc0),
        ("imm", 0x7e3323, "c4e37d19c001"),
        ("imm", 0x7e3329, "c4e3f916c101"),
        ("imm", 0x7e334b, "48c7839000000000000000"),
        ("imm", 0x7e3356, "ff5020"),
        ("call", 0x7e335e, 0xf590),
        ("call", 0x7e3368, 0xf590),
        ("rip", 0x7e336d, 7, 3, 0x4b0),
        ("rip", 0x7e3374, 7, 3, 0x1604218),
        ("call", 0x7e337e, 0xf8f390),
        ("rip", 0x7e3383, 7, 3, 0x15fe518),
        ("call", 0x7e33a6, 0xf8f3a0),
    ],
    "m6_callees": {
        'name_holder_dtor': (0x8ef00, 23, "32799b7bccf74b604c28f179e15a8bfb5b928f37bfacc3efa336f33d022e6c86"),
        'handler_tag_7def00': (0x7def00, 73, "5073f62434453872ab7805527cd3decbfe2869bf5af401bc902d0afe1829588f"),
    },
    "m6_extras": ['bp_atexit_imports', 'bp_holder_vtables', 'fcmd_vtable'],
    "native": {'header': 'native/core/ctor_slot_339_7e2fc0.hpp', 'source': 'native/core/ctor_slot_339_7e2fc0.cpp', 'test': 'native/tests/test_ctor_slot_339.cpp', 'consts': {'kSlot339Rva': 8269760, 'kSlot339Size': 1004, 'kSlot339FloatFnTag': 22864960, 'kSlot339HandlerTag': 8253184, 'kSlot339HolderDtorRva': 585472, 'kSlot339HolderVptr': 22640304, 'kSlot339Holder0NameLength': 14, 'kSlot339Holder1NameLength': 28, 'kSlot339FloatNameLength': 28, 'kSlot339FloatValueBits': 1123024896, 'kSlot339CommandDescriptionLength': 47, 'kSlot339CellCount': 7, 'kSlot339Cell0Rva': 52821936, 'kSlot339Holder0Rva': 52822048, 'kSlot339Holder1Rva': 52822096, 'kSlot339FloatRva': 52822144, 'kSlot339CommandRva': 52822368},
        'header_tokens': ['"CurrentPlayTimeEASY"', '"CurrentPlayTimeNORMAL"', '"CurrentPlayTimeHARD"', '"FastestPlayTimeEASY"', '"FastestPlayTimeNORMAL"', '"FastestPlayTimeHARD"', '"CurrentDeathCount"', 'kSlot339Holder0Name[] = "shadowDemoMode"', 'kSlot339Holder1Name[] = "shadowDemoMode_gameplayIntro"', 'kSlot339FloatName[] = "shadowDemoMode_gameplayTimer"', 'kSlot339CommandName[] = "killBoss"', 'kSlot339CommandDescription[] = "Kills the current boss if present. Params: None"', 'IDENTITY TAGS'], 'source_order': ['cells[n].hash = fnv_cstr_52a990(kSlot339CellNames[n])', 'holder.vptr = kSlot339HolderVptr', 'release_inert_view_f590(view)', 'registry.atexit_log.record(kSlot339HolderDtorRva, &holder)', 'build_holder(objects.holder0', 'build_holder(objects.holder1', 'helper_init_unbounded_float_var_site(objects.fvar, spec, concmd)', 'init_fconcmd_object(objects.command, cmd, concmd)'], 'test_tokens': ['kSlot339Rva == 0x7e2fc0 && kSlot339Size == 1004', 'section_alone()', 'section_failures()', 'r.ok && r.cells_ok && r.holders_ok && r.refusal == HelperRefusal::none', 'CHECK(creg.pending.size() == 2', 'CHECK(creg.atexit_log.size() == 4);', 'check_atexit(creg, 0, 0x8ef00, &o.holder0)', 'check_atexit(creg, 1, 0x8ef00, &o.holder1)', 'check_atexit(creg, 3, 0x4b0, &o.command)', 'fnv_cstr_52a990(kSlot339CellNames[n])', 'HelperRefusal::unadmitted_arg_functor']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'seven fnv1a name cells + two 0x30 B name holders (atexit 0x8ef00) + unbounded float var (tag 0x15ce440) + console command killBoss (flags 2, atexit 0x4b0)'))
