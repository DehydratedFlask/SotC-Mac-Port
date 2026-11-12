#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 340 (RVA 0x7de7e0, 1800 bytes), batch 10 (the startup-table cliff of batch 9, flagged "two indirect calls, XL").

Static resolution of the six indirect calls (read with objdump over the FDE range; the chain is pinned below, the whole body is hash-pinned): `leaq 0x1594dc0(%rip),%rax` (0x7de888) -> `movq 0x20(%rax),%r13`
(0x7de8bf, the ONLY load of r13 before the spill) -> `callq *%r13` at 0x7de8dd / 0x7de9bd / 0x7dea9d / 0x7deb82; `movq %r13,0x38(%rsp)` (0x7deb6a, the only write of that slot, r13 is callee-saved across the
intervening direct calls) -> `callq *0x38(%rsp)` at 0x7dec85 / 0x7dee8e. Every call therefore targets the console-command class table entry [0x1594dc0 + 0x20], whose RELA record is pinned to 0x20d60 (the register-once
body, hash-pinned as concmd_register). No indirect target is unresolved; nothing is emulated or executed.

Shape: four console commands shadow_gameflag{chk,dump,set,cls} (empty descriptions, flags 2, handler tags 0x7db650 / 0x7db740 / 0x7db870 / 0x7db990, objects 0x325f090 + 0xa0 n), a NameHashCell
{TimeAttackSetBestTime, fnv1a} at 0x325f858, the command bp_giveAllOrbs (0x325fd20, handler 0x7ddc60), the menu group CP10 / `Shadow Cheat` (0x325fdc0, 0x26c10), the 0x26ea0 toggle `Shadow Cheat` / `Give All Orbs`
(0x325fe68, target = the bp_giveAllOrbs command object r13 = 0x325fd20, flags 0x30) and the command bp_dumpCp3GameStateInfo (0x325ff10, handler 0x7dddd0). Atexit: 0x4b0 per command, 0x272d0 per menu item. The toggle's
range stays {1.0, 10.0} because the command class (vptr 0x1594dd0) answers virtual slot 3 with 0x22ae0 `xor eax,eax; ret` (pinned: the `cmd_class_gate` extra), so the {0.1, 0.5} gate of 0x26ea0 never opens.

The handler bodies are hash-pinned by FDE extent; vtables, handler tags and destructors are IDENTITY TAGS (recorded, never run). The game-flag / orbs / cp3-dump reading is inference from the strings.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

from slot_pin_util import require  # noqa: E402


def x_cmd_class_gate(e, spec) -> None:
    """Console command class 0x1594dd0: slot 0 = 0x4b0, slot 1 = 0x22a60, slot 2 = 0x20d60, slot 3 = 0x22ae0 (`xor eax,eax; ret`, so 0x26ea0 never reads slot 6)."""
    for index, target in enumerate((0x4B0, 0x22A60, 0x20D60, 0x22AE0)):
        require(e.reloc_target(0x1594DD0 + 8 * index) == target, f"console command vtable 0x1594dd0 slot {index} -> {target:#x}")
    e.bytes_at(0x22AE0, "31c0c3", "command slot 3: xor eax,eax; ret")


bp.lib.EXTRAS["cmd_class_gate"] = x_cmd_class_gate

SPEC = {
    "slot": 340,
    "base": 0x7de7e0,
    "size": 1800,
    "body_sha256": "d195e5d9d7387a47e46e4c58207553d332b5c6d0aa7957fd2dfa33197cc9b772",
    "pad": "9090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'release_f590', 'cmd_dtor', 'menu_item_dtor', 'menu_toggle_ctor'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0x1289b90, b'shadow_gameflagchk'),
        (0x1289ba3, b''),
        (0x1289ba4, b'shadow_gameflagdump'),
        (0x1289bb8, b'shadow_gameflagset'),
        (0x1289bcb, b'shadow_gameflagcls'),
        (0x1289c0c, b'TimeAttackSetBestTime'),
        (0x1289c22, b'bp_giveAllOrbs'),
        (0x1289c31, b'Gives the user all the current difficulty orbs.'),
        (0x1289c61, b'CP10'),
        (0x1289c66, b'Shadow Cheat'),
        (0x1289c73, b'Give All Orbs'),
        (0x1289c81, b'bp_dumpCp3GameStateInfo'),
        (0x1289c99, b'Dump cp3 game state related information so it can be inspected and compared'),
    ],
    "lengths": [
        ('Gives the user all the current difficulty orbs.', 0x7dec1a, "48c74424682f000000"),
        ('CP10', 0x7decef, "48c784248800000004000000"),
        ('Shadow Cheat', 0x7ded00, "48c74424680c000000"),
        ('Shadow Cheat', 0x7ded79, "48c78424880000000c000000"),
        ('Give All Orbs', 0x7ded90, "48c74424680d000000"),
        ('Dump cp3 game state related information so it can be inspected and compared', 0x7dee1a, "48c74424684b000000"),
    ],
    "pins": [
        ("rip", 0x7de7f8, 7, 3, 0x15fe518),
        ("rip", 0x7de807, 7, 3, 0x1289b90),
        ("call", 0x7de81e, 0xf2f0),
        ("rip", 0x7de823, 7, 3, 0x325f090),
        ("rip", 0x7de82a, 8, 4, 0x15fe5e0),
        ("rip", 0x7de832, 7, 3, 0x1289ba3),
        ("rip", 0x7de842, 7, 3, 0x7db650),
        ("imm", 0x7de84e, "b902000000"),
        ("imm", 0x7de866, "48c744247800000000"),
        ("call", 0x7de883, 0x20af0),
        ("rip", 0x7de888, 7, 3, 0x1594dc0),
        ("imm", 0x7de8a1, "c4e37d19c001"),
        ("imm", 0x7de8ac, "c4e3f916c101"),
        ("imm", 0x7de8d2, "48c7839000000000000000"),
        ("imm", 0x7de8dd, "41ffd5"),
        ("call", 0x7de8e5, 0xf590),
        ("call", 0x7de8f2, 0xf590),
        ("rip", 0x7de8f7, 7, 3, 0x4b0),
        ("rip", 0x7de8fe, 7, 3, 0x1604218),
        ("call", 0x7de908, 0xf8f390),
        ("rip", 0x7de915, 7, 3, 0x1289ba4),
        ("call", 0x7de921, 0xf2f0),
        ("rip", 0x7de926, 7, 3, 0x325f130),
        ("rip", 0x7de937, 7, 3, 0x7db740),
        ("imm", 0x7de943, "b902000000"),
        ("imm", 0x7de95b, "48c744247800000000"),
        ("call", 0x7de972, 0x20af0),
        ("imm", 0x7de98a, "c4e37d19c001"),
        ("imm", 0x7de990, "c4e3f916c001"),
        ("imm", 0x7de9b2, "48c7839000000000000000"),
        ("imm", 0x7de9bd, "41ffd5"),
        ("call", 0x7de9c5, 0xf590),
        ("call", 0x7de9d2, 0xf590),
        ("rip", 0x7de9d7, 7, 3, 0x4b0),
        ("rip", 0x7de9de, 7, 3, 0x1604218),
        ("call", 0x7de9e8, 0xf8f390),
        ("rip", 0x7de9f5, 7, 3, 0x1289bb8),
        ("call", 0x7dea01, 0xf2f0),
        ("rip", 0x7dea06, 7, 3, 0x325f1d0),
        ("rip", 0x7dea17, 7, 3, 0x7db870),
        ("imm", 0x7dea23, "b902000000"),
        ("imm", 0x7dea3b, "48c744247800000000"),
        ("call", 0x7dea52, 0x20af0),
        ("imm", 0x7dea6a, "c4e37d19c001"),
        ("imm", 0x7dea70, "c4e3f916c001"),
        ("imm", 0x7dea92, "48c7839000000000000000"),
        ("imm", 0x7dea9d, "41ffd5"),
        ("call", 0x7deaa5, 0xf590),
        ("call", 0x7deab2, 0xf590),
        ("rip", 0x7deab7, 7, 3, 0x4b0),
        ("rip", 0x7deabe, 7, 3, 0x1604218),
        ("call", 0x7deac8, 0xf8f390),
        ("rip", 0x7dead5, 7, 3, 0x1289bcb),
        ("call", 0x7deae1, 0xf2f0),
        ("rip", 0x7deae6, 7, 3, 0x325f270),
        ("rip", 0x7deaf7, 7, 3, 0x7db990),
        ("imm", 0x7deb03, "b902000000"),
        ("imm", 0x7deb1b, "48c744247800000000"),
        ("call", 0x7deb32, 0x20af0),
        ("imm", 0x7deb4a, "c4e37d19c001"),
        ("imm", 0x7deb50, "c4e3f916c001"),
        ("imm", 0x7deb77, "48c7839000000000000000"),
        ("imm", 0x7deb82, "41ffd5"),
        ("call", 0x7deb8a, 0xf590),
        ("call", 0x7deb97, 0xf590),
        ("rip", 0x7deb9c, 7, 3, 0x4b0),
        ("rip", 0x7deba3, 7, 3, 0x1604218),
        ("call", 0x7debad, 0xf8f390),
        ("rip", 0x7debb2, 7, 3, 0x325f858),
        ("rip", 0x7debb9, 7, 3, 0x1289c0c),
        ("call", 0x7debc3, 0x52a990),
        ("rip", 0x7debd0, 7, 3, 0x1289c22),
        ("call", 0x7debdf, 0xf2f0),
        ("rip", 0x7debe4, 7, 3, 0x325fd20),
        ("rip", 0x7debf1, 7, 3, 0x1289c31),
        ("imm", 0x7dec01, "b902000000"),
        ("rip", 0x7dec13, 7, 3, 0x7ddc60),
        ("imm", 0x7dec1a, "48c74424682f000000"),
        ("call", 0x7dec37, 0x20af0),
        ("imm", 0x7dec51, "c4e37d19c001"),
        ("imm", 0x7dec57, "c4e3f916c001"),
        ("imm", 0x7dec7a, "49c7859000000000000000"),
        ("imm", 0x7dec85, "ff542438"),
        ("call", 0x7dec8e, 0xf590),
        ("call", 0x7dec9b, 0xf590),
        ("rip", 0x7deca0, 7, 3, 0x4b0),
        ("rip", 0x7deca7, 7, 3, 0x1604218),
        ("call", 0x7decb1, 0xf8f390),
        ("rip", 0x7decb6, 7, 3, 0x325fdc0),
        ("rip", 0x7decca, 7, 3, 0x1289c61),
        ("rip", 0x7decd1, 7, 3, 0x1289c66),
        ("imm", 0x7decdc, "b930000000"),
        ("imm", 0x7decef, "48c784248800000004000000"),
        ("imm", 0x7ded00, "48c74424680c000000"),
        ("call", 0x7ded1b, 0x26c10),
        ("call", 0x7ded23, 0xf590),
        ("call", 0x7ded2b, 0xf590),
        ("rip", 0x7ded30, 7, 3, 0x272d0),
        ("rip", 0x7ded37, 7, 3, 0x1604218),
        ("call", 0x7ded44, 0xf8f390),
        ("rip", 0x7ded49, 7, 3, 0x325fe68),
        ("rip", 0x7ded65, 7, 3, 0x1289c73),
        ("imm", 0x7ded70, "41b830000000"),
        ("imm", 0x7ded79, "48c78424880000000c000000"),
        ("imm", 0x7ded90, "48c74424680d000000"),
        ("call", 0x7dedab, 0x26ea0),
        ("call", 0x7dedb3, 0xf590),
        ("call", 0x7dedbb, 0xf590),
        ("rip", 0x7dedc0, 7, 3, 0x1604218),
        ("call", 0x7dedcd, 0xf8f390),
        ("rip", 0x7dedda, 7, 3, 0x1289c81),
        ("call", 0x7dede6, 0xf2f0),
        ("rip", 0x7dedeb, 7, 3, 0x325ff10),
        ("rip", 0x7dedf8, 7, 3, 0x1289c99),
        ("imm", 0x7dee08, "b902000000"),
        ("imm", 0x7dee1a, "48c74424684b000000"),
        ("rip", 0x7dee2e, 7, 3, 0x7dddd0),
        ("call", 0x7dee43, 0x20af0),
        ("imm", 0x7dee5b, "c4e37d19c001"),
        ("imm", 0x7dee61, "c4e3f916c001"),
        ("imm", 0x7dee83, "48c7839000000000000000"),
        ("imm", 0x7dee8e, "ff542438"),
        ("call", 0x7dee97, 0xf590),
        ("call", 0x7deea4, 0xf590),
        ("rip", 0x7deea9, 7, 3, 0x4b0),
        ("rip", 0x7deeb0, 7, 3, 0x1604218),
        ("call", 0x7deeba, 0xf8f390),
        ("rip", 0x7deebf, 7, 3, 0x15fe518),
        ("call", 0x7deee2, 0xf8f3a0),
        ("imm", 0x7de895, "488d4810"),
        ("imm", 0x7de8a7, "48894c2418"),
        ("imm", 0x7de8bf, "4c8b6820"),
        ("imm", 0x7deb6a, "4c896c2438"),
    ],
    "m6_callees": {
        'handler_tag_7db650': (0x7db650, 234, "d8f44db5dda2485266bbee6d4623babc6c78fdbb569f5ee93d86898d2d7d0498"),
        'handler_tag_7db740': (0x7db740, 299, "5b58e00c94c974932b176dc17d708cbd0f716ef14c2b91f82565236ca5d9c831"),
        'handler_tag_7db870': (0x7db870, 288, "314afcfc44fe86ed4c6e27aacfea4858c54d90b65ed34e7c109eba760234623b"),
        'handler_tag_7db990': (0x7db990, 288, "f4f80fbfbd8986937557ed5b9377819e1689fa931ea7907c5d4adb0a373865ea"),
        'handler_tag_7ddc60': (0x7ddc60, 360, "bd07d7fe1ce87454b9e97cc4f2eb0f0aa3e57959b81027f47c28c9b5938c7daf"),
        'handler_tag_7dddd0': (0x7dddd0, 13, "1b277594e41bb7e25017da97239f544f62d0e9b46278e4462b383c6ddde2722c"),
        'fnv_cstr_52a990': (0x52a990, 10, "0a633e2d30c9e38a4b9829a318723764d2c4cbeccaa47330c39bfcce9bd5d9f0"),
        'menu_group_ctor': (0x26c10, 194, "cc1c2bcce3014d52d61f88a2ac0b3c6a0666198cba7527d46d3e2ebaf03114fe"),
        'menu_register_268b0': (0x268b0, 679, "5930a19a609c50b3ba26a927c6dff432df81bc4377b7e2f3c7704735065fb44f"),
    },
    "m6_extras": ['bp_atexit_imports', 'fcmd_vtable', 'toggle_gate', 'cmd_class_gate'],
    "native": {'header': 'native/core/ctor_slot_340_7de7e0.hpp', 'source': 'native/core/ctor_slot_340_7de7e0.cpp', 'test': 'native/tests/test_ctor_slot_340.cpp', 'consts': {'kSlot340Rva': 8251360, 'kSlot340Size': 1800, 'kSlot340ItemFlags': 48, 'kSlot340CommandGate3': 0, 'kSlot340CommandCount': 6, 'kSlot340CellRva': 52820056, 'kSlot340GroupRva': 52821440, 'kSlot340ToggleRva': 52821608, 'kSlot340GroupParentLength': 4, 'kSlot340GroupCaptionLength': 12, 'kSlot340ToggleTextLength': 13},
        'header_tokens': ['{"shadow_gameflagchk", "", 0, 0x7db650, 0x325f090}', '{"shadow_gameflagdump", "", 0, 0x7db740, 0x325f130}', '{"shadow_gameflagset", "", 0, 0x7db870, 0x325f1d0}', '{"shadow_gameflagcls", "", 0, 0x7db990, 0x325f270}', '{"bp_giveAllOrbs", "Gives the user all the current difficulty orbs.", 0x2f, 0x7ddc60, 0x325fd20}', '{"bp_dumpCp3GameStateInfo", "Dump cp3 game state related information so it can be inspected and compared", 0x4b, 0x7dddd0, 0x325ff10}', 'kSlot340CellName[] = "TimeAttackSetBestTime"', 'kSlot340GroupParent[] = "CP10"', 'kSlot340GroupCaption[] = "Shadow Cheat"', 'kSlot340ToggleText[] = "Give All Orbs"', 'IDENTITY TAGS', '0x22ae0'], 'source_order': ['bool build_command(Slot340Result& out', 'cmd.base.vptr == kFConCmdVptr', 'n < 4; ++n', 'objects.cell.hash = fnv_cstr_52a990(kSlot340CellName)', 'build_command(out, objects, 4, concmd)', 'family_group_item_26c10(objects.group', 'family_toggle_item_26ea0(objects.toggle', 'command_menu_target(objects.cmd[4])', 'build_command(out, objects, 5, concmd)'], 'test_tokens': ['kSlot340Rva == 0x7de7e0 && kSlot340Size == 1800', 'section_alone()', 'section_failures()', 'r.ok && r.rows_done == 9', 'CHECK(creg.pending.size() == 6', 'CHECK(creg.atexit_log.size() == 9);', 'check_atexit(creg, 5, 0x27280, &menu.root)', 'check_atexit(creg, 6, 0x272d0, &o.group)', 'check_atexit(creg, 7, 0x272d0, &o.toggle)', 'check_atexit(creg, 8, 0x4b0, &o.cmd[5])', 'o.toggle.target == &o.cmd[4]', 'o.toggle.range_min == 1.0f && o.toggle.range_max == 10.0f', 'menu.root.count == 2', 'creg.manager.manager = &token', 'menu.parent.parent = &token']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'six console commands (r13 / 0x38(%rsp) register-once dispatch resolved to 0x20d60) + name cell + menu group + 0x26ea0 toggle over a command object'))
