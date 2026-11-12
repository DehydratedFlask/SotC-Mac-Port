#!/usr/bin/env python3
"""ELF byte-oracle for the debug-menu slider constructor 0x27120 (0xd2 B), the "text from the target variable" sibling of 0x27050 (startup slots 349, 351, 370, 375, 380, 392, 396, 402 call it; slot 392 is modelled).

0x27120(this=rdi, label=rsi, target=rdx, flags=ecx, min=xmm0, max=xmm1) writes the 0x27050 item stores in the same order, but copies the TEXT view from the target object's name view (target + 0x20 .. 0x40, with `lock inc` of the control counter when non-null)
instead of an argument, stores the target pointer from rdx, flags from ecx, and tail-calls 0x268b0(this, [0x2dfd4b8]). Every call, rip operand and immediate of the body is pinned and the store windows are byte-pinned.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": -1,
    "label": 'helper_27120',
    "no_table": True,
    "base": 0x27120,
    "size": 210,
    "body_sha256": "eea78ecfab62fc92ee5aaf58e9df756f8382f3ab4eb598c524ef3b3e30c24f07",
    "pad": "9090909090909090909090909090",
    "callees": [],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
    ],
    "lengths": [
    ],
    "pins": [
        ("imm", 0x2713d, "48c70300000000"),
        ("imm", 0x2714c, "48c7431800000000"),
        ("imm", 0x27154, "c6432000"),
        ("imm", 0x2717a, "c7434802000000"),
        ("call", 0x271a9, 0xf180),
        ("rip", 0x271b5, 7, 3, 0x2dfd4b8),
        ("jmp", 0x271ed, 0x268b0),
    ],
    "m6_callees": {
        'slider_ctor_27050': (0x27050, 208, "ad7ca05e3df035fa5df336f0463fb63e331566f140d6f0bf3ad16e9e2766b053"),
        'slider_from_var_ctor_27120': (0x27120, 210, "eea78ecfab62fc92ee5aaf58e9df756f8382f3ab4eb598c524ef3b3e30c24f07"),
        'menu_register_268b0': (0x268b0, 679, "5930a19a609c50b3ba26a927c6dff432df81bc4377b7e2f3c7704735065fb44f"),
    },
    "m6_bytes": [
        (0x27138, "89ce4989d748c7030000000048895b0848895b1048c7431800000000c6", 'item: link {0, this, this, 0} and registered byte = 0'),
        (0x27158, "c5f81006c5f8114328488b461048894338488b4618488943404885c07404f048ff00", 'item: label view copied from the label argument (rsi) with lock inc of its control counter'),
        (0x2717a, "c7434802000000", 'item: kind = 2'),
        (0x27181, "c4c178104720c5f8114350498b473048894360498b4738488943684885c07404f048ff00", "item: TEXT view copied from the TARGET variable's name view (target + 0x20) with lock inc of its control counter"),
        (0x271ae, "4c89bb90000000", 'item: target = rdx (the variable), not a text argument'),
        (0x271bf, "c5fa1045e4c5fa118398000000c5fa1045e0c5fa11839c0000004489b3a000", 'item: range min / max from xmm0 / xmm1'),
        (0x271d9, "4489b3a0000000", 'item: flags = ecx'),
        (0x271ed, "e9bef6ffff", 'tail jmp 0x268b0'),
    ],
    "m6_extras": [],
    "native": {'header': 'native/core/debug_menu_item.hpp', 'source': 'native/core/debug_menu_item.cpp', 'test': 'native/tests/test_ctor_slot_392.cpp', 'consts': {'kDebugMenuItemFloatFromVarCtorRva': 160032, 'kDebugMenuItemFloatFromVarCtorSize': 210},
        'header_tokens': ['init_debug_menu_item_27120(DebugMenuItem& item, const SharedStr& label, const ConCmdBase& target,'], 'source_order': ['construct_common(item, label, target.name, &target, flags);', 'item.range_min = range_min;   // [0x98] = xmm0 (0x27120)', 'item.range_max = range_max;   // [0x9c] = xmm1 (0x27120)'], 'test_tokens': ['section_item_27120', 'CHECK(init_debug_menu_item_27120(item, label, v.base, 0x30, 0.25f, 0.5f));', 'same_view(item.text, kSlot392Names[0], kSlot392NameLengths[0])']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'slider ctor 0x27120 (text = the target variable name view) hash-pinned, all calls/operands/store windows pinned'))
