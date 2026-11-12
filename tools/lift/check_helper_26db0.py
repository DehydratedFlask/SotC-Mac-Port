#!/usr/bin/env python3
"""ELF-tie checker for the class-aware debug-menu named-item front end (CPU-01-HELPER-CALLEE-MODELS-01, 0x26db0).

Pins 0x26db0 (0xef B, FDE 239) instruction by instruction, including the two virtual reads `call [vptr+0x18]` /
`call [vptr+0x30]` and the {1.0, 10.0} default / {0.1, 0.5} narrowing, the class slots 3 and 6 of the four variable
classes the adapter names (bool 0x1594218, int 0x1594c08, float 0x15942b0, string 0x1594ef0: relocation targets and
the `mov eax, imm; ret` bodies), and the two 0x26db0 call sites of startup slots 366 (0x744ac0) and 411 (0x591710),
whose targets are the bool class only (no other variable class table is loaded in either body).

Usage: python3 -I tools/lift/check_helper_26db0.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402
from slot_pin_util import require  # noqa: E402

# class base (the table the constructors lea, final vptr = base + 0x10)
BOOL_BASE, INT_BASE, FLOAT_BASE, STRING_BASE = 0x1594208, 0x1594BF8, 0x15942A0, 0x1594EE0
SITES = [  # slot 366 / 411: base, size, FDE, sha, the two calls to 0x26db0, the bool-class lea
    ("slot 366 (0x744ac0)", 0x744AC0, 695, "2aec5eb2ce229433641565f2b6edee8e471b8c833a4abe8be069c67b1856460c",
     (0x744C01, 0x744D33), 0x744AD8),
    ("slot 411 (0x591710)", 0x591710, 692, "b7deef0503476aea502554adb743740fbf5203f363fa46001572ac9df9836ebe",
     (0x59192D, 0x591984), 0x591728),
]


def extra(e, fams) -> None:
    forbidden = {INT_BASE, FLOAT_BASE, STRING_BASE, INT_BASE + 0x10, FLOAT_BASE + 0x10, STRING_BASE + 0x10}
    for name, base, size, sha, calls, lea in SITES:
        e.body_hash(name, base, size, sha, always=True)
        for site in calls:
            require(e.call(site) == 0x26DB0, f"{name}: call at {site:#x} reaches 0x26db0")
        require(e.lea(lea) == BOOL_BASE, f"{name}: the target class table loaded at {lea:#x} is the bool class")
        body = e.raw(base, size)
        found = [base + i for i in range(size - 4) if body[i] == 0xE8
                 and base + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] == 0x26DB0]
        require(found == sorted(calls), f"{name}: exactly two calls to 0x26db0, got {[hex(x) for x in found]}")
        loaded = set()
        for i in range(size - 6):
            if body[i] in (0x48, 0x4C) and body[i + 1] == 0x8D and (body[i + 2] & 0xC7) == 0x05:
                loaded.add(base + i + 7 + struct.unpack("<i", body[i + 3:i + 7])[0])
        require(not (loaded & forbidden), f"{name}: loads an int/float/string variable class table "
                                          f"{sorted(hex(x) for x in loaded & forbidden)}")


CLASS_RELOCS = []
CLASS_BYTES = []
for _cls, _final, _s3, _s6, _v3, _v6 in (("bool", 0x1594218, 0xB890, 0xB930, "b801000000c3", "31c0c3"),
                                          ("int", 0x1594C08, 0xBF70, 0xBFC0, "b801000000c3", "b802000000c3"),
                                          ("float", 0x15942B0, 0xBBA0, 0xBBF0, "b801000000c3", "b803000000c3"),
                                          ("string", 0x1594EF0, 0x246A0, 0x24780, "b801000000c3", "b804000000c3")):
    CLASS_RELOCS += [(f"{_cls} class slot 3 -> {_s3:#x}", _final + 0x18, _s3),
                     (f"{_cls} class slot 6 -> {_s6:#x}", _final + 0x30, _s6)]
    CLASS_BYTES += [(f"{_cls} class slot 3 body", _s3, _v3), (f"{_cls} class slot 6 body", _s6, _v6)]

SPEC = {
    "name": "check_helper_26db0",
    "summary": ("0x26db0 (0xef B, sha 903ee574f904...) pinned per instruction, class slots 3/6 of the bool / int / "
                "float / string classes, slots 366/411 call it with bool targets only"),
    "families": [],
    "bodies": [
        {"name": "0x26db0 named menu item ctor", "rva": 0x26DB0, "size": 0xEF, "fde": 239, "pins": "menu_26db0",
         "sha": "903ee574f90468eb5c2ed01f6d299b010dbafd8ed3d90f18ed9a283754a62c6e"},
    ],
    "rips": [("0x26db0 tail registry lea", 0x26E83, 7, 3, 0x2DFD4B8)],
    "calls": [("0x26db0 empty-slot init 0xf180", 0x26E2F, 0xF180)],
    "jmps": [("0x26db0 tail jump 0x268b0", 0x26E9A, 0x268B0)],
    "relocs": CLASS_RELOCS,
    "bytes": CLASS_BYTES,
    "extra": extra,
    "native": {
        "tags_h": "native/core/helper_functor_tags.hpp", "helper_h": "native/core/helper_26db0_named_item.hpp",
        "helper_c": "native/core/helper_26db0_named_item.cpp", "menu_h": "native/core/menu_item_26db0.hpp",
        "menu_c": "native/core/menu_item_26db0.cpp", "dmi_h": "native/core/debug_menu_item.hpp",
        "int_h": "native/core/ranged_int_var_c580.hpp", "float_h": "native/core/lane_m6_shapes.hpp",
        "str_h": "native/core/string_var_257a0.hpp", "test": "native/tests/test_helper_26db0.cpp",
        "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "menu_h": {"kMenuNamedItemCtorRva": 0x26DB0, "kMenuNamedItemCtorEndRva": 0x26E9F,
                   "kMenuNamedItemCtorSize": 0xEF},
        "dmi_h": {"kBoolVarSlot3Result": 1, "kBoolVarSlot6Result": 0},
        "int_h": {"kRangedIntSlot3Result": 1, "kRangedIntSlot6Result": 2, "kRangedIntFinalVptr": 0x1594C08},
        "float_h": {"kRangedFloatSlot3Result": 1, "kRangedFloatSlot6Result": 3},
        "str_h": {"kStringVarSlot3Result": 1, "kStringVarSlot6Result": 4, "kStringVarFinalVptr": 0x1594EF0},
    },
    "tokens": {
        "menu_c": ["if (!target.known) return false;", "item.range_min = 1.0f;", "item.range_max = 10.0f;",
                   "if (target.slot3 == 1 && target.slot6 == 3) {", "item.range_min = 0.1f;",
                   "item.range_max = 0.5f;", "item.target = &target_base;", "init_slot_f180(&item.empty);"],
        "helper_c": ["if (!target.known) return HelperRefusal::unknown_target_class;",
                     "init_debug_menu_item_26db0(item, parent, base, target, flags)",
                     "build(item, parent, target.base, debug_menu_target_for(target), flags)",
                     "build(item, parent, target.base, ranged_int_c580_menu_target(target), flags)",
                     "build(item, parent, target.base, lane_m6::ranged_float_menu_target(target), flags)",
                     "build(item, parent, target.base, string_var_menu_target(target), flags)"],
        "test": ["check_item(item, parent, caption, &v, 1.0f, 10.0f, 0x30);",
                 "check_item(item, parent, caption, &v, 1.0f, 10.0f, 0x31);",
                 "check_item(item, parent, caption, &v, 0.1f, 0.5f, 0x32);",
                 "check_item(item, parent, caption, &v, 1.0f, 10.0f, 0x33);"],
        "cmake": ["core/helper_26db0_named_item.cpp", "add_test(NAME helper_26db0 COMMAND test_helper_26db0)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
