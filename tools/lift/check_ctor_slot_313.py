#!/usr/bin/env python3
"""ELF-tie startup constructor slot 313 (0xa447a0): `update_entitlement_status` console command, the
"Entitlement System" group and "Update Entitlement State" item of the "Platform Services" menu.

Offline only: opens the pinned private ELF (local/re/eboot.elf), reads bytes, never loads, relocates or
executes an original RVA. Pins: ELF sha256, constructor-table word 313, the 661-byte body (sha256, extent,
padding), every call / rip operand / immediate of the body, the strings, the reused callees by hash, the
command vtable 0x1594dc0 slots, the invoker-thunk cell, and the native header / source / test / CMake
tokens. Mutation self-test and `--tamper elf|native` (exits 1) come from l2_slot_check.py.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from l2_slot_check import Spec, main, require  # noqa: E402

BASE = 0xA447A0
SIZE = 661
SHA = "24c825f06612efc849f23c46ac17abe1735cc176f819638d5f810a3438eef134"

PINS = [
    ('bytes', 0xa447ad, '4883e4e0'),
    ('bytes', 0xa447b1, '4881ec80000000'),
    ('rip', 0xa447b8, 7, 3, 0x15fe518),
    ('rip', 0xa447bf, 7, 3, 0x1604218),
    ('rip', 0xa447c6, 7, 3, 0x32f6d18),
    ('rip', 0xa447cd, 7, 3, 0xa42900),
    ('bytes', 0xa447df, '48c70600000000'),
    ('call', 0xa447e6, 0xf8f390),
    ('rip', 0xa447eb, 7, 3, 0xf7b0),
    ('rip', 0xa447f2, 7, 3, 0x12c2ce0),
    ('rip', 0xa447f9, 7, 3, 0x32f6d28),
    ('rip', 0xa44807, 7, 3, 0x32f6d28),
    ('rip', 0xa4480e, 11, 3, 0x32f6d30),
    ('bytes', 0xa44815, '12000000'),
    ('rip', 0xa44819, 8, 4, 0x32f6d38),
    ('call', 0xa44821, 0xf8f390),
    ('rip', 0xa4482b, 7, 3, 0x12c2cf3),
    ('call', 0xa44837, 0xf2f0),
    ('rip', 0xa4483c, 8, 4, 0x15fe5e0),
    ('rip', 0xa44844, 7, 3, 0x12c2d0d),
    ('rip', 0xa4484f, 7, 3, 0x32f6d48),
    ('bytes', 0xa4485b, 'b902000000'),
    ('bytes', 0xa4486c, '48c74424281e000000'),
    ('rip', 0xa4487f, 7, 3, 0xa42930),
    ('call', 0xa44891, 0x20af0),
    ('rip', 0xa44896, 7, 3, 0x1594dc0),
    ('rip', 0xa448a6, 7, 3, 0x32f6d48),
    ('rip', 0xa448ad, 8, 4, 0x32f6da8),
    ('bytes', 0xa448b5, 'c4e37d19c001'),
    ('bytes', 0xa448bb, 'c4e3f916c101'),
    ('bytes', 0xa448c4, '740e'),
    ('rip', 0xa448c6, 7, 3, 0x32f6da8),
    ('rip', 0xa448cd, 7, 3, 0x32f6da8),
    ('rip', 0xa448d4, 7, 3, 0x32f6d48),
    ('rip', 0xa448df, 8, 4, 0x32f6dc8),
    ('rip', 0xa448e7, 11, 3, 0x32f6dd8),
    ('bytes', 0xa448ee, '00000000'),
    ('call', 0xa448fd, 0xf590),
    ('call', 0xa44907, 0xf590),
    ('rip', 0xa4490c, 7, 3, 0x1604218),
    ('rip', 0xa44913, 7, 3, 0x4b0),
    ('call', 0xa44920, 0xf8f390),
    ('rip', 0xa44925, 7, 3, 0x32f6de8),
    ('rip', 0xa4492c, 7, 3, 0x32f6d28),
    ('rip', 0xa44938, 7, 3, 0x12c2d2c),
    ('bytes', 0xa44943, 'b930000000'),
    ('bytes', 0xa44956, '48c744244811000000'),
    ('call', 0xa44965, 0x26c10),
    ('call', 0xa4496d, 0xf590),
    ('rip', 0xa44972, 7, 3, 0x272d0),
    ('call', 0xa44982, 0xf8f390),
    ('rip', 0xa44987, 7, 3, 0x32f6e90),
    ('rip', 0xa44993, 7, 3, 0x12c2d3e),
    ('bytes', 0xa4499e, '41b830000000'),
    ('rip', 0xa449a7, 7, 3, 0x32f6d48),
    ('bytes', 0xa449b9, '48c744244818000000'),
    ('call', 0xa449c8, 0x26ea0),
    ('call', 0xa449d0, 0xf590),
    ('call', 0xa449de, 0xf8f390),
    ('rip', 0xa449e3, 7, 3, 0xa42950),
    ('rip', 0xa449ee, 7, 3, 0x32f6f38),
    ('rip', 0xa449f8, 8, 4, 0x32f6f38),
    ('rip', 0xa44a00, 11, 3, 0x32f6f48),
    ('bytes', 0xa44a07, '00000000'),
    ('call', 0xa44a0b, 0xf8f390),
    ('rip', 0xa44a10, 7, 3, 0x15fe518),
    ('bytes', 0xa44a1f, '750f'),
    ('bytes', 0xa44a2f, 'c3'),
    ('call', 0xa44a30, 0xf8f3a0),
]

CALLEES = {
    "concmd_base_ctor": (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45"),
    "concmd_register": (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "menu_group_ctor": (0x26C10, 0xC2, "cc1c2bcce3014d52d61f88a2ac0b3c6a0666198cba7527d46d3e2ebaf03114fe"),
    "menu_toggle_ctor": (0x26EA0, 0xED, "c2c7d8c17db68783d9e94100c4f032f3bf06c1ac7d12d5f48e0e34e06a924677"),
    "menu_item_dtor": (0x272D0, 0x19E, "41ea4aa9e998a114e13cd7149228ae5c542debbeb084c1d412a988f2e801c770"),
    "fconcmd_dtor": (0x4B0, 0x71, "a15bae38e545531a8c5769a079333bd4e0385846160e10a4b768827ecb0d8c8c"),
    "invoke_thunk": (0x1990, 0xC, "c5eca1d65aea48ea6feaa7c41abef0c4f4d7bc1212e753f2d12da5b89285fd12"),
    "command_slot3": (0x22AE0, 0x3, "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf"),
}


def elf_extra(e) -> None:
    # strings (all four views carry immediate lengths that the pins above fix: 0x12, 0x1e, 0x11, 0x18)
    e.cstring(0x12C2CE0, b"Entitlement System")
    e.cstring(0x12C2CF3, b"update_entitlement_status")
    e.cstring(0x12C2D0D, b"Updates the entitlement status")
    e.cstring(0x12C2D2C, b"Platform Services")
    e.cstring(0x12C2D3E, b"Update Entitlement State")
    # the command class: vtable 0x1594dc0, address point 0x1594dd0; slot 2 is the register guard, slot 3
    # returns 0 so the 0x26ea0 toggle keeps its default range
    for i, rva in enumerate((0x4B0, 0x22A60, 0x20D60, 0x22AE0)):
        require(e.reloc_target(0x1594DD0 + 8 * i) == rva, f"command vtable slot {i} = {rva:#x}")
    e.bytes_at(0x22AE0, "31c0c3", "command slot 3: xor eax,eax; ret")
    require(e.reloc_target(0x15FE5E0) == 0x1990, "delegate invoker cell 0x15fe5e0 -> 0x1990")
    e.bytes_at(0x1990, "488b4718" "4889f7" "4889d6" "ffe0", "invoker thunk body")
    # object placement is contiguous with the native layout
    require(0x32F6D48 + 0xA0 == 0x32F6DE8 and 0x32F6DE8 + 0xA8 == 0x32F6E90 and 0x32F6E90 + 0xA8 == 0x32F6F38,
            "object placement")


SPEC = Spec(
    name="slot313", index=313, base=BASE, size=SIZE, sha=SHA, pins=PINS, callees=CALLEES, elf_extra=elf_extra,
    native_files={"header": "native/core/ctor_slot_313_a447a0.hpp", "source": "native/core/ctor_slot_313_a447a0.cpp",
                  "test": "native/tests/test_ctor_slot_313.cpp", "cmake": "native/CMakeLists.txt"},
    header_constants={"kSlot313Rva": BASE, "kSlot313BodySize": SIZE, "kSlot313CellRva": 0x32F6D18,
                      "kSlot313CellDtorRva": 0xA42900, "kSlot313CaptionRva": 0x32F6D28,
                      "kSlot313CaptionDtorRva": 0xF7B0, "kSlot313CommandRva": 0x32F6D48,
                      "kSlot313HandlerRva": 0xA42930, "kSlot313GroupRva": 0x32F6DE8,
                      "kSlot313ToggleRva": 0x32F6E90, "kSlot313TailRva": 0x32F6F38,
                      "kSlot313TailDtorRva": 0xA42950, "kSlot313ItemFlags": 0x30,
                      "kSlot313CaptionLength": 0x12, "kSlot313DescriptionLength": 0x1E},
    header_tokens=['kSlot313CommandName[] = "update_entitlement_status"', 'kSlot313Caption[] = "Entitlement System"',
                   'kSlot313Parent[] = "Platform Services"', 'kSlot313Text[] = "Update Entitlement State"',
                   'kSlot313Description[] = "Updates the entitlement status"', "struct Slot313Objects"],
    source_order=["kSlot313CellDtorRva, &o.cell", "kSlot313CaptionDtorRva, &o.caption", "init_fconcmd_object(o.command",
                  "init_menu_group_26c10(o.group, parent, o.caption, kSlot313ItemFlags)",
                  "register_menu_item_268b0(o.group", "release_inert_view_f590(parent)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.group)",
                  "init_debug_menu_item_26ea0(o.toggle, o.caption, text, l2::command_menu_target(o.command)",
                  "register_menu_item_268b0(o.toggle", "release_inert_view_f590(text)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle)", "kSlot313TailDtorRva, &o.tail"],
    test_tokens=["kSlot313Rva == 0xa447a0", "section_build()", "section_failures()", "{0x272d0, &o.group}"],
    cmake_tokens=["core/ctor_slot_313_a447a0.cpp", "add_executable(test_ctor_slot_313 tests/test_ctor_slot_313.cpp)",
                  "add_test(NAME ctor_slot_313 COMMAND test_ctor_slot_313)"],
    summary="entitlement command (flags 2, handler tag 0xa42930) + group + toggle, atexit 0xa42900/0xf7b0/0x4b0/0x272d0 x2/0xa42950",
)

if __name__ == "__main__":
    raise SystemExit(main(SPEC))
