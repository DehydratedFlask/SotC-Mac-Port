#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 369 (RVA 0x734190, 898 bytes), batch 6 debug page.

Bool console variable `cp10_enableWanderBonePhysics` (static/literal shape, value = default = 1, tag 0x15cd160, object 0x32509b0), a 0x26ea0 toggle for it (item 0x3250a78, parent `CP10-Anim`, caption
`Enable Wander Bone Physics`, flags 0x30), an UNBOUNDED float variable `cp10_WanderBonePhysicsCollisionRadius` (0x6aa00, flags 1, value = default = 0x3e19999a, caller functor tag 0x15cd180,
object 0x3250b20) and a 0x27050 slider (item 0x3250bf8, caption `Wander Bone Physics Collision Radius`, flags 0x30, range {0x3c23d70a, 0x3dcccccd} read from .rodata 0x127d1bc / 0x127d1c0).

0x27050 is the native init_debug_menu_item_27050 (also tied by check_ctor_slot_361 / 467 / check_fctor_slots_298_299_301); here its 208 byte body hash, the kind=2 store and the tail jump
to 0x268b0 are re-pinned. The float caller functor tag is one of the 68 float-family class tables; the native slot reaches 0x6aa00 through helper_6aa00_unbounded_float and requires
HelperRefusal::none. What reads the variables at run time is NOT established (the Wander Bone Physics reading is inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test;
--tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 369,
    "base": 0x734190,
    "size": 898,
    "body_sha256": "985ad2285888cba8b758b0903df8a0c3f8bb7fdabcb1ce8cb96158f3436736fe",
    "pad": "9090909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6', 'menu_toggle_ctor', 'menu_item_dtor', 'unbounded_float_ctor', 'float_dtor', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy'],
    "bool_tags": [0x15cd160],
    "float_tags": [0x15cd180],
    "strings": [
        (0x127d220, b'cp10_enableWanderBonePhysics'),
        (0x127d23d, b'CP10-Anim'),
        (0x127d247, b'Enable Wander Bone Physics'),
        (0x127d262, b'cp10_WanderBonePhysicsCollisionRadius'),
        (0x127d288, b'Wander Bone Physics Collision Radius'),
        (0x127d3b8, b''),
    ],
    "lengths": [
        ('cp10_enableWanderBonePhysics', 0x734200, "48c74424281c000000"),
        ('CP10-Anim', 0x734309, "48c744244809000000"),
        ('Enable Wander Bone Physics', 0x734312, "48c74424281a000000"),
        ('cp10_WanderBonePhysicsCollisionRadius', 0x7343ad, "48c744240825000000"),
        ('CP10-Anim', 0x734490, "48c744244809000000"),
        ('Wander Bone Physics Collision Radius', 0x734499, "48c744242824000000"),
    ],
    "pins": [
        ("rip", 0x7341a8, 7, 3, 0x15fe518),
        ("rip", 0x7341af, 7, 3, 0x1594208),
        ("rip", 0x7341b6, 7, 3, 0x32509b0),
        ("rip", 0x7341bd, 7, 3, 0x127d220),
        ("rip", 0x7341c4, 7, 3, 0x127d3b8),
        ("imm", 0x7341d9, "b901000000"),
        ("imm", 0x7341e4, "4883c310"),
        ("imm", 0x734200, "48c74424281c000000"),
        ("rip", 0x734215, 7, 3, 0x32509b0),
        ("imm", 0x73421c, "48c744245800000000"),
        ("call", 0x734225, 0x20af0),
        ("rip", 0x73422a, 7, 3, 0x15940a0),
        ("rip", 0x734234, 7, 3, 0x3250a70),
        ("rip", 0x73423f, 7, 3, 0x15cd160),
        ("imm", 0x734246, "4883c010"),
        ("rip", 0x73424a, 7, 3, 0x32509b0),
        ("rip", 0x734251, 7, 3, 0x3250a10),
        ("rip", 0x734258, 10, 2, 0x3250a19),
        ("imm", 0x734258, "c705b7c7b10200000000"),
        ("rip", 0x734262, 8, 4, 0x3250a20),
        ("rip", 0x73426a, 11, 3, 0x3250a50),
        ("imm", 0x73426a, "48c705dbc7b10200000000"),
        ("rip", 0x734275, 7, 3, 0x3250a60),
        ("rip", 0x73427c, 11, 3, 0x3250a68),
        ("imm", 0x73427c, "48c705e1c7b10200000000"),
        ("call", 0x734287, 0xf590),
        ("rip", 0x73428f, 7, 3, 0x32509b0),
        ("rip", 0x734296, 7, 2, 0x3250a70),
        ("imm", 0x734296, "c605d3c7b10201"),
        ("rip", 0x73429d, 7, 2, 0x3250a18),
        ("imm", 0x73429d, "c60574c7b10201"),
        ("call", 0x7342a4, 0x20d60),
        ("call", 0x7342ac, 0xf590),
        ("rip", 0x7342b1, 7, 3, 0x1604218),
        ("rip", 0x7342b8, 7, 3, 0x20b0),
        ("call", 0x7342c5, 0xf8f390),
        ("rip", 0x7342ca, 7, 3, 0x127d247),
        ("rip", 0x7342d1, 7, 3, 0x3250a78),
        ("rip", 0x7342e2, 7, 3, 0x127d23d),
        ("imm", 0x7342ed, "41b830000000"),
        ("imm", 0x734309, "48c744244809000000"),
        ("imm", 0x734312, "48c74424281a000000"),
        ("call", 0x734327, 0x26ea0),
        ("call", 0x73432f, 0xf590),
        ("call", 0x734337, 0xf590),
        ("rip", 0x73433c, 7, 3, 0x272d0),
        ("call", 0x734349, 0xf8f390),
        ("rip", 0x73434e, 7, 3, 0x15942a0),
        ("rip", 0x734355, 7, 3, 0x127d262),
        ("rip", 0x73435c, 7, 3, 0x3250b20),
        ("imm", 0x734370, "48c744247000000000"),
        ("rip", 0x73437d, 7, 3, 0x3250bf0),
        ("imm", 0x734389, "b901000000"),
        ("rip", 0x734397, 7, 3, 0x15cd180),
        ("imm", 0x7343ad, "48c744240825000000"),
        ("rip", 0x7343c4, 7, 3, 0x127d3b8),
        ("imm", 0x7343cb, "48c784248800000000000000"),
        ("imm", 0x7343e2, "48c744243800000000"),
        ("imm", 0x7343eb, "4983c710"),
        ("rip", 0x7343ef, 7, 3, 0x3250b20),
        ("call", 0x7343f6, 0x6aa00),
        ("call", 0x7343fe, 0xf590),
        ("imm", 0x73440e, "ff5018"),
        ("rip", 0x734414, 7, 3, 0x3250b20),
        ("imm", 0x73441b, "c705cbc7b1029a99193e"),
        ("imm", 0x734425, "c70559c7b1029a99193e"),
        ("call", 0x73442f, 0x20d60),
        ("call", 0x734437, 0xf590),
        ("rip", 0x73443c, 7, 3, 0x1604218),
        ("rip", 0x734443, 7, 3, 0x20f0),
        ("call", 0x734450, 0xf8f390),
        ("rip", 0x734455, 7, 3, 0x127d23d),
        ("rip", 0x73445c, 7, 3, 0x127d288),
        ("rip", 0x734467, 8, 4, 0x127d1c0),
        ("rip", 0x73446f, 7, 3, 0x3250bf8),
        ("imm", 0x734480, "41b830000000"),
        ("imm", 0x734490, "48c744244809000000"),
        ("imm", 0x734499, "48c744242824000000"),
        ("rip", 0x7344ae, 8, 4, 0x127d1bc),
        ("call", 0x7344c2, 0x27050),
        ("call", 0x7344ca, 0xf590),
        ("call", 0x7344d2, 0xf590),
        ("rip", 0x7344d7, 7, 3, 0x272d0),
        ("call", 0x7344e4, 0xf8f390),
        ("rip", 0x7344e9, 7, 3, 0x15fe518),
        ("call", 0x73450c, 0xf8f3a0),
    ],
    "m6_callees": {
        'menu_float_ctor_27050': (0x27050, 208, "ad7ca05e3df035fa5df336f0463fb63e331566f140d6f0bf3ad16e9e2766b053"),
    },
    "m6_bytes": [
        (0x270aa, "c7434802000000", '0x27050 kind = 2'),
        (0x2711b, "e990f7ffff", '0x27050 tail jmp 0x268b0'),
        (0x127d1bc, "0ad7233c", 'slider min 0x3c23d70a (.rodata)'),
        (0x127d1c0, "cdcccc3d", 'slider max 0x3dcccccd (.rodata)'),
    ],
    "m6_extras": ['bp_atexit_imports', 'bp_object_vtables'],
    "native": {'header': 'native/core/ctor_slot_369_734190.hpp', 'source': 'native/core/ctor_slot_369_734190.cpp', 'test': 'native/tests/test_ctor_slot_369.cpp', 'consts': {'kSlot369Rva': 7553424, 'kSlot369Size': 898, 'kSlot369BoolFnTag': 22860128, 'kSlot369FloatFnTag': 22860160, 'kSlot369Var0NameLength': 28, 'kSlot369ParentLength': 9, 'kSlot369Item0TextLength': 26, 'kSlot369Var1NameLength': 37, 'kSlot369Var1ValueBits': 1041865114, 'kSlot369Item1TextLength': 36, 'kSlot369Item1MinBits': 1008981770, 'kSlot369Item1MaxBits': 1036831949, 'kSlot369ItemFlags': 48, 'kSlot369Var0Rva': 52758960, 'kSlot369Item0Rva': 52759160, 'kSlot369Var1Rva': 52759328, 'kSlot369Item1Rva': 52759544},
        'header_tokens': ['kSlot369Var0Name[] = "cp10_enableWanderBonePhysics"', 'kSlot369Parent[] = "CP10-Anim"', 'kSlot369Item0Text[] = "Enable Wander Bone Physics"', 'kSlot369Var1Name[] = "cp10_WanderBonePhysicsCollisionRadius"', 'kSlot369Item1Text[] = "Wander Bone Physics Collision Radius"', 'IDENTITY TAGS'], 'source_order': ['lane_m6::init_bool_var_literal(objects.var0,', 'lane_m6::init_toggle_site_target(objects.item0,', 'helper_init_unbounded_float_var_site(objects.var1, spec, concmd)', 'init_debug_menu_item_27050(objects.item1, label, text, &objects.var1,', 'register_debug_menu_item_268b0(objects.item1, menu, concmd.atexit_log)', 'release_inert_view_f590(text) && release_inert_view_f590(label)', 'concmd.atexit_log.record(kDebugMenuItemDtorRva, &objects.item1)'], 'test_tokens': ['kSlot369Rva == 0x734190 && kSlot369Size == 898', 'section_alone()', 'section_refusal()', 'section_failures()', 'CHECK(creg.atexit_log.size() == 5);', 'CHECK(r.ok && r.refusal == HelperRefusal::none);', 'HelperRefusal::unadmitted_arg_functor']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'bool var + 0x26ea0 toggle + unbounded float var (0x6aa00, tag 0x15cd180) + 0x27050 slider (range 0.01..0.1)'))
