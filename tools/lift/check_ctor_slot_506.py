#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 506 (RVA 0x2340b0, 860 bytes), batch 7 debug page.

UNBOUNDED int variable `drawTouch` (the twin constructor 0x233ec0, flags 1, value = default = 0, empty description, caller functor tag 0x159b680 = the twin's private functor class, object 0x2ee94a0, final class 0x159b618, atexit 0x230d60),
UNBOUNDED int variable `checkTouch` (0x3a980, value = default = 0, caller functor tag 0x159b6a0 of the int family, object 0x2ee9578, atexit 0x18a60), a 0x26c10 group (empty parent, caption `Touch Manager`, item 0x2ee9650)
and one 0x26ea0 toggle `Draw` (item 0x2ee96f8) whose target is the first variable.

Callee ties: the twin constructor 0x233ec0 is checked by check_helper_233ec0.py and hash-pinned here; its final class 0x159b618 answers slot 3 = slot 6 = 1, so the 0x26ea0 narrowing gate (`slot3 == 1 && slot6 == 3`) does not fire and the
default range {1.0, 10.0} stays. The second variable's tag 0x159b6a0 has the int family's four slot bodies (int_site). The vtables, functor tags and destructors (0x230d60, 0x18a60, 0x272d0) are IDENTITY TAGS (recorded, never run).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test;
--tamper [elf|native] are explicit rejection probes. What reads the variables at run time is NOT established (the touch-manager reading is inference from the strings).
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402


from slot_pin_util import require  # noqa: E402


def x_twin_classes(e, spec) -> None:
    """The first variable's classes: final 0x159b618 (dtor 0x230d60, slot 3 = slot 6 = 1) and the private functor class 0x159b680 used as the caller tag."""
    for index, target in ((0, 0x230D60), (2, 0x20D60), (3, 0x2334C0), (6, 0x233510)):
        require(e.reloc_target(0x159B618 + 8 * index) == target, f"twin final vtable 0x159b618 slot {index}")
    for index, target in enumerate((0x234050, 0x234070, 0x234090, 0x2340A0)):
        require(e.reloc_target(0x159B680 + 8 * index) == target, f"private functor table 0x159b680 slot {index}")
    require(e.rip(0x234054, 7, 3) == 0x159B680, "private functor copy stores class 0x159b680")
    require(e.rip(0x234070, 7, 3) == 0x159B680, "private functor clone stores class 0x159b680")
    e.bytes_at(0x2340A0, "c3", "private functor destroy is a bare ret")
    e.bytes_at(0x2334C0, "b801000000c3", "twin slot 3: mov eax,1; ret")
    e.bytes_at(0x233510, "b801000000c3", "twin slot 6: mov eax,1; ret")


bp.lib.EXTRAS["twin_classes"] = x_twin_classes

SPEC = {
    "slot": 506,
    "base": 0x2340b0,
    "size": 860,
    "body_sha256": "af4dbbf34978aefd095e0660aa1f812a2d51dfbe7092ce2ebe9b0bec6de24b4b",
    "pad": "90909090",
    "callees": ['concmd_register', 'release_f590', 'menu_toggle_ctor', 'menu_item_dtor'],
    "bool_tags": [],
    "float_tags": [],
    "m6_int_tags": [0x159b6a0],
    "strings": [
        (0xfc0240, b'drawTouch'),
        (0xfc024a, b'checkTouch'),
        (0xfc0255, b''),
        (0xfc0256, b'Touch Manager'),
        (0xfc0264, b'Draw'),
    ],
    "lengths": [
        ('drawTouch', 0x23412b, "48c744240809000000"),
        ('checkTouch', 0x23423d, "48c74424080a000000"),
        ('Touch Manager', 0x234320, "48c74424280d000000"),
        ('Touch Manager', 0x234390, "48c74424480d000000"),
        ('Draw', 0x2343a7, "48c744242804000000"),
    ],
    "pins": [
        ("rip", 0x2340c8, 7, 3, 0x15fe518),
        ("rip", 0x2340cf, 7, 3, 0x159b608),
        ("rip", 0x2340d6, 7, 3, 0xfc0240),
        ("rip", 0x2340dd, 7, 3, 0x2ee94a0),
        ("rip", 0x2340f1, 7, 3, 0x159b680),
        ("imm", 0x234101, "b901000000"),
        ("imm", 0x234114, "4883c310"),
        ("rip", 0x234124, 7, 3, 0xfc0255),
        ("imm", 0x23412b, "48c744240809000000"),
        ("imm", 0x23413a, "48c744247000000000"),
        ("rip", 0x234158, 7, 3, 0x2ee94a0),
        ("imm", 0x23415f, "48c784248800000000000000"),
        ("rip", 0x234170, 7, 3, 0x2ee9570),
        ("imm", 0x23417d, "48c744243800000000"),
        ("call", 0x234186, 0x233ec0),
        ("call", 0x23418e, 0xf590),
        ("imm", 0x23419e, "ff5018"),
        ("rip", 0x2341a4, 7, 3, 0x2ee94a0),
        ("rip", 0x2341ab, 10, 2, 0x2ee9570),
        ("imm", 0x2341ab, "c705bb53cb0200000000"),
        ("rip", 0x2341b5, 10, 2, 0x2ee9508),
        ("imm", 0x2341b5, "c7054953cb0200000000"),
        ("call", 0x2341bf, 0x20d60),
        ("call", 0x2341c7, 0xf590),
        ("rip", 0x2341cc, 7, 3, 0x230d60),
        ("rip", 0x2341d3, 7, 3, 0x1604218),
        ("call", 0x2341dd, 0xf8f390),
        ("rip", 0x2341e2, 7, 3, 0x1594bf8),
        ("rip", 0x2341e9, 7, 3, 0xfc024a),
        ("rip", 0x2341f0, 7, 3, 0x2ee9578),
        ("rip", 0x234200, 7, 3, 0xfc0255),
        ("rip", 0x23420f, 7, 3, 0x2ee9648),
        ("imm", 0x23421b, "b901000000"),
        ("imm", 0x234220, "48c744247000000000"),
        ("rip", 0x23422d, 7, 3, 0x159b6a0),
        ("imm", 0x23423d, "48c74424080a000000"),
        ("imm", 0x23425d, "48c744243800000000"),
        ("imm", 0x23426e, "48c784248800000000000000"),
        ("imm", 0x23427a, "4983c710"),
        ("rip", 0x23427e, 7, 3, 0x2ee9578),
        ("call", 0x234285, 0x3a980),
        ("call", 0x23428d, 0xf590),
        ("imm", 0x23429f, "ff5018"),
        ("rip", 0x2342a5, 7, 3, 0x2ee9578),
        ("rip", 0x2342ac, 10, 2, 0x2ee9648),
        ("imm", 0x2342ac, "c7059253cb0200000000"),
        ("rip", 0x2342b6, 10, 2, 0x2ee95e0),
        ("imm", 0x2342b6, "c7052053cb0200000000"),
        ("call", 0x2342c0, 0x20d60),
        ("call", 0x2342c8, 0xf590),
        ("rip", 0x2342d0, 7, 3, 0x1604218),
        ("rip", 0x2342d7, 7, 3, 0x18a60),
        ("call", 0x2342e1, 0xf8f390),
        ("rip", 0x2342eb, 7, 3, 0x2ee9650),
        ("rip", 0x234300, 7, 3, 0xfc0256),
        ("imm", 0x234307, "b930000000"),
        ("imm", 0x234320, "48c74424280d000000"),
        ("imm", 0x234329, "48c744245800000000"),
        ("call", 0x234338, 0x26c10),
        ("call", 0x234340, 0xf590),
        ("call", 0x234348, 0xf590),
        ("rip", 0x23434d, 7, 3, 0x272d0),
        ("call", 0x23435d, 0xf8f390),
        ("rip", 0x234362, 7, 3, 0x2ee96f8),
        ("rip", 0x234373, 7, 3, 0xfc0264),
        ("imm", 0x23437e, "41b830000000"),
        ("rip", 0x234384, 7, 3, 0x2ee94a0),
        ("imm", 0x234390, "48c74424480d000000"),
        ("imm", 0x2343a7, "48c744242804000000"),
        ("call", 0x2343bc, 0x26ea0),
        ("call", 0x2343c4, 0xf590),
        ("call", 0x2343cc, 0xf590),
        ("rip", 0x2343d1, 7, 3, 0x1604218),
        ("call", 0x2343de, 0xf8f390),
        ("rip", 0x2343e3, 7, 3, 0x15fe518),
        ("call", 0x234406, 0xf8f3a0),
    ],
    "m6_callees": {
        'twin_ctor_233ec0': (0x233ec0, 322, "8cc4bc88431dd5584596794b315d60cf7b7fb8fc409af98f1f201b3c0389e427"),
        'twin_fn_copy_234050': (0x234050, 20, "44dc6354c1fd08ec8c83fdb1a7975cacf3055c42958d4b907bb80eab7b1ade3c"),
        'twin_fn_clone_234070': (0x234070, 20, "59dfb39fe29b291ab5a856eba4f50b3110add8f269e30f8424a0f7be48526e60"),
        'twin_fn_move_234090': (0x234090, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
        'twin_fn_destroy_2340a0': (0x2340a0, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
        'twin_slot3_2334c0': (0x2334c0, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
        'twin_slot6_233510': (0x233510, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
        'twin_dtor_230d60': (0x230d60, 54, "cb1d411473cb1562581dea8ee0c88e70eb7ae80036f57595bef6df94f10b1af0"),
        'int_var_worker_3a980': (0x3a980, 322, "2cbfc2763abb6506a59551b902dd6098325889e5e520d842c0e93da7f43498ca"),
        'int_dtor_18a60': (0x18a60, 54, "4de4f3993b02c84c9c20824690e8ab9d4f7a1b84babfa6cbe0d89c9dd558b50b"),
        'int_fn_copy_c720': (0xc720, 20, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
        'int_fn_clone_c740': (0xc740, 20, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
        'int_fn_move_c760': (0xc760, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
        'int_fn_destroy_c770': (0xc770, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
        'int_slot3_bf70': (0xbf70, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
        'int_slot6_bfc0': (0xbfc0, 6, "7140f35dee6220b79b12aecc27acf5105bf3b77d1588e89fce345de7c16c72b7"),
        'menu_group_ctor_26c10': (0x26c10, 194, "cc1c2bcce3014d52d61f88a2ac0b3c6a0666198cba7527d46d3e2ebaf03114fe"),
    },
    "m6_extras": ['bp_atexit_imports', 'int_site', 'toggle_gate', 'twin_classes'],
    "native": {'header': 'native/core/ctor_slot_506_2340b0.hpp', 'source': 'native/core/ctor_slot_506_2340b0.cpp', 'test': 'native/tests/test_ctor_slot_506.cpp', 'consts': {'kSlot506Rva': 2310320, 'kSlot506Size': 860, 'kSlot506Var0FnTag': 22656640, 'kSlot506Var1FnTag': 22656672, 'kSlot506Var0NameLength': 9, 'kSlot506Var1NameLength': 10, 'kSlot506GroupCaptionLength': 13, 'kSlot506ItemTextLength': 4, 'kSlot506ItemFlags': 48, 'kSlot506Var0Rva': 49190048, 'kSlot506Var1Rva': 49190264, 'kSlot506GroupRva': 49190480, 'kSlot506ItemRva': 49190648},
        'header_tokens': ['kSlot506Var0Name[] = "drawTouch"', 'kSlot506Var1Name[] = "checkTouch"', 'kSlot506GroupCaption[] = "Touch Manager"', 'kSlot506ItemText[] = "Draw"', 'IDENTITY TAGS'], 'source_order': ['helper_init_unbounded_int_twin_site(objects.var0, spec0, concmd)', 'helper_init_unbounded_int_var_site(objects.var1, spec1, concmd)', 'family_group_item_26c10(objects.group, "", 0,', 'lane_m6::init_toggle_site_target(objects.item, unbounded_int_twin_menu_target(objects.var0)'], 'test_tokens': ['kSlot506Rva == 0x2340b0 && kSlot506Size == 860', 'section_alone()', 'section_failures()', 'CHECK(creg.atexit_log.size() == 5);', 'CHECK(r.ok && r.refusal == HelperRefusal::none);', 'CHECK(menu.root.count == 2);', 'CHECK(creg.pending.size() == 2);']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'twin unbounded int drawTouch (0x233ec0, private functor class 0x159b680) + unbounded int checkTouch (0x3a980, tag 0x159b6a0) + 0x26c10 group `Touch Manager` + one 0x26ea0 toggle `Draw`'))
