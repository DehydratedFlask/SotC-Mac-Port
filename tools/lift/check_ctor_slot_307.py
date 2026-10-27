#!/usr/bin/env python3
"""ELF-tie startup constructor slot 307 (0xa5b090): the "Trophy System" block of the "Platform Services"
debug menu (trophyProcessingEnabled bool variable, unlockTrophy / unlockAllTrophies commands, group and
two toggles).

Offline only: opens the pinned private ELF (local/re/eboot.elf), reads bytes, never loads, relocates or
executes an original RVA. Pins: ELF sha256, constructor-table word 307, the 1288-byte body (sha256,
extent, padding), every call / rip operand / immediate of the body, the strings, the reused callees by
hash, the bool and command class slots, the functor vtable 0x15d1dc0 and the native header / source /
test / CMake tokens. Mutation self-test and `--tamper elf|native` (exits 1) come from l2_slot_check.py.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from l2_slot_check import Spec, main, require  # noqa: E402

BASE = 0xA5B090
SIZE = 1288
SHA = "601d471d5041d7d1c0cf7423739e92a655d844304c3e7e73ee4ff9b256db90d6"

PINS = [
    ('bytes', 0xa5b09d, '4883e4e0'),
    ('bytes', 0xa5b0a1, '4881eca0000000'),
    ('rip', 0xa5b0a8, 7, 3, 0x15fe518),
    ('rip', 0xa5b0af, 7, 3, 0x1604218),
    ('rip', 0xa5b0b6, 7, 3, 0x32f8b80),
    ('rip', 0xa5b0bd, 7, 3, 0xa59370),
    ('bytes', 0xa5b0d2, '48c70600000000'),
    ('call', 0xa5b0d9, 0xf8f390),
    ('rip', 0xa5b0de, 7, 3, 0x1594208),
    ('rip', 0xa5b0e5, 7, 3, 0x12c47d0),
    ('rip', 0xa5b0ec, 7, 3, 0x32f8b90),
    ('bytes', 0xa5b101, 'b901000000'),
    ('rip', 0xa5b10b, 7, 3, 0x12c48d0),
    ('bytes', 0xa5b11b, '48c744244817000000'),
    ('bytes', 0xa5b135, '48c744247800000000'),
    ('bytes', 0xa5b13e, '4883c310'),
    ('rip', 0xa5b142, 7, 3, 0x32f8b90),
    ('call', 0xa5b149, 0x20af0),
    ('rip', 0xa5b14e, 7, 3, 0x15940a0),
    ('rip', 0xa5b158, 7, 3, 0x32f8c50),
    ('rip', 0xa5b163, 7, 3, 0x15d1dc0),
    ('bytes', 0xa5b16a, '4883c010'),
    ('rip', 0xa5b16e, 7, 3, 0x32f8b90),
    ('rip', 0xa5b175, 7, 3, 0x32f8bf0),
    ('rip', 0xa5b17c, 10, 2, 0x32f8bf9),
    ('bytes', 0xa5b182, '00000000'),
    ('rip', 0xa5b186, 8, 4, 0x32f8c00),
    ('rip', 0xa5b18e, 11, 3, 0x32f8c30),
    ('bytes', 0xa5b195, '00000000'),
    ('rip', 0xa5b199, 7, 3, 0x32f8c40),
    ('rip', 0xa5b1a0, 11, 3, 0x32f8c48),
    ('bytes', 0xa5b1a7, '00000000'),
    ('call', 0xa5b1ab, 0xf590),
    ('rip', 0xa5b1b0, 7, 3, 0x32f8b90),
    ('rip', 0xa5b1ba, 7, 2, 0x32f8c50),
    ('bytes', 0xa5b1c0, '00'),
    ('rip', 0xa5b1c1, 7, 2, 0x32f8bf8),
    ('bytes', 0xa5b1c7, '00'),
    ('call', 0xa5b1cb, 0x20d60),
    ('call', 0xa5b1d3, 0xf590),
    ('rip', 0xa5b1d8, 7, 3, 0x20b0),
    ('call', 0xa5b1e5, 0xf8f390),
    ('rip', 0xa5b1ea, 7, 3, 0xf7b0),
    ('rip', 0xa5b1f1, 7, 3, 0x32f8c58),
    ('rip', 0xa5b1f8, 7, 3, 0x12c47e8),
    ('rip', 0xa5b209, 7, 3, 0x32f8c58),
    ('rip', 0xa5b210, 11, 3, 0x32f8c60),
    ('bytes', 0xa5b217, '0d000000'),
    ('rip', 0xa5b21b, 8, 4, 0x32f8c68),
    ('call', 0xa5b223, 0xf8f390),
    ('rip', 0xa5b228, 7, 3, 0x32f8c78),
    ('rip', 0xa5b234, 7, 3, 0x12c47f6),
    ('bytes', 0xa5b23f, 'b930000000'),
    ('bytes', 0xa5b252, '48c744246811000000'),
    ('call', 0xa5b261, 0x26c10),
    ('call', 0xa5b269, 0xf590),
    ('rip', 0xa5b26e, 7, 3, 0x272d0),
    ('call', 0xa5b27e, 0xf8f390),
    ('rip', 0xa5b283, 7, 3, 0x32f8d20),
    ('rip', 0xa5b28f, 7, 3, 0x12c4808),
    ('bytes', 0xa5b29a, '41b830000000'),
    ('rip', 0xa5b2a3, 7, 3, 0x32f8b90),
    ('bytes', 0xa5b2b5, '48c744246812000000'),
    ('call', 0xa5b2c4, 0x26ea0),
    ('call', 0xa5b2cc, 0xf590),
    ('call', 0xa5b2da, 0xf8f390),
    ('rip', 0xa5b2df, 7, 3, 0xa593a0),
    ('rip', 0xa5b2ea, 7, 3, 0x32f8dc8),
    ('rip', 0xa5b2f4, 8, 4, 0x32f8dc8),
    ('rip', 0xa5b2fc, 11, 3, 0x32f8dd8),
    ('bytes', 0xa5b303, '00000000'),
    ('call', 0xa5b307, 0xf8f390),
    ('rip', 0xa5b30c, 7, 3, 0xa593b0),
    ('rip', 0xa5b317, 7, 3, 0x32f8de8),
    ('rip', 0xa5b321, 8, 4, 0x32f8de8),
    ('rip', 0xa5b329, 11, 3, 0x32f8df8),
    ('bytes', 0xa5b330, '00000000'),
    ('call', 0xa5b334, 0xf8f390),
    ('rip', 0xa5b33e, 7, 3, 0x12c481b),
    ('call', 0xa5b34a, 0xf2f0),
    ('rip', 0xa5b34f, 7, 3, 0x32f8e08),
    ('rip', 0xa5b356, 8, 4, 0x15fe5e0),
    ('rip', 0xa5b35e, 7, 3, 0x12c4828),
    ('bytes', 0xa5b373, 'b902000000'),
    ('rip', 0xa5b380, 7, 3, 0xa59410),
    ('bytes', 0xa5b387, '48c74424482d000000'),
    ('call', 0xa5b3af, 0x20af0),
    ('rip', 0xa5b3b4, 7, 3, 0x1594dc0),
    ('bytes', 0xa5b3cd, 'c4e37d19c001'),
    ('bytes', 0xa5b3d3, 'c4e3f916c101'),
    ('bytes', 0xa5b3dc, '7408'),
    ('bytes', 0xa5b3f9, '48c7839000000000000000'),
    ('call', 0xa5b40c, 0xf590),
    ('call', 0xa5b416, 0xf590),
    ('rip', 0xa5b41b, 7, 3, 0x4b0),
    ('rip', 0xa5b422, 7, 3, 0x1604218),
    ('call', 0xa5b42c, 0xf8f390),
    ('rip', 0xa5b436, 7, 3, 0x12c4856),
    ('call', 0xa5b442, 0xf2f0),
    ('rip', 0xa5b447, 7, 3, 0x32f8ea8),
    ('rip', 0xa5b454, 7, 3, 0x12c4868),
    ('bytes', 0xa5b464, 'b902000000'),
    ('rip', 0xa5b476, 7, 3, 0xa595c0),
    ('bytes', 0xa5b47d, '48c744244815000000'),
    ('call', 0xa5b49a, 0x20af0),
    ('bytes', 0xa5b4ad, 'c4e37d19c001'),
    ('bytes', 0xa5b4b3, 'c4e3f916c001'),
    ('bytes', 0xa5b4bc, '7408'),
    ('bytes', 0xa5b4d5, '48c7839000000000000000'),
    ('call', 0xa5b4e8, 0xf590),
    ('call', 0xa5b4f2, 0xf590),
    ('rip', 0xa5b4f7, 7, 3, 0x1604218),
    ('rip', 0xa5b4fe, 7, 3, 0x4b0),
    ('call', 0xa5b50b, 0xf8f390),
    ('rip', 0xa5b510, 7, 3, 0x32f8f48),
    ('rip', 0xa5b51c, 7, 3, 0x12c487e),
    ('rip', 0xa5b527, 7, 3, 0x32f8c58),
    ('bytes', 0xa5b52e, '41b830000000'),
    ('bytes', 0xa5b542, '48c744246813000000'),
    ('call', 0xa5b551, 0x26ea0),
    ('call', 0xa5b559, 0xf590),
    ('rip', 0xa5b55e, 7, 3, 0x272d0),
    ('call', 0xa5b56b, 0xf8f390),
    ('rip', 0xa5b570, 7, 3, 0x15fe518),
    ('bytes', 0xa5b582, '750f'),
    ('bytes', 0xa5b592, 'c3'),
    ('call', 0xa5b593, 0xf8f3a0),
]

CALLEES = {
    "concmd_base_ctor": (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45"),
    "concmd_register": (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "bool_dtor": (0x20B0, 0x36, "73f646e91ca2f7d8dd14cf1858c93197eddbf00a4267f6ca36ea00b2500efe42"),
    "menu_group_ctor": (0x26C10, 0xC2, "cc1c2bcce3014d52d61f88a2ac0b3c6a0666198cba7527d46d3e2ebaf03114fe"),
    "menu_toggle_ctor": (0x26EA0, 0xED, "c2c7d8c17db68783d9e94100c4f032f3bf06c1ac7d12d5f48e0e34e06a924677"),
    "menu_item_dtor": (0x272D0, 0x19E, "41ea4aa9e998a114e13cd7149228ae5c542debbeb084c1d412a988f2e801c770"),
    "fconcmd_dtor": (0x4B0, 0x71, "a15bae38e545531a8c5769a079333bd4e0385846160e10a4b768827ecb0d8c8c"),
    "invoke_thunk": (0x1990, 0xC, "c5eca1d65aea48ea6feaa7c41abef0c4f4d7bc1212e753f2d12da5b89285fd12"),
    "bool_slot3": (0xB890, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "bool_slot6": (0xB930, 0x3, "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf"),
    "command_slot3": (0x22AE0, 0x3, "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf"),
}


def elf_extra(e) -> None:
    for addr, text in ((0x12C47D0, b"trophyProcessingEnabled"), (0x12C47E8, b"Trophy System"),
                       (0x12C47F6, b"Platform Services"), (0x12C4808, b"Processing Enabled"),
                       (0x12C481B, b"unlockTrophy"), (0x12C4828, b"Unlocks a trophy. +unlockTrophy <trophy_name>"),
                       (0x12C4856, b"unlockAllTrophies"), (0x12C4868, b"Unlocks all trophies."),
                       (0x12C487E, b"Unlock All Trophies")):
        e.cstring(addr, text)
    require(e.read(0x12C48D0, 1) == b"\0", "empty description points at a NUL byte")
    # bool class 0x1594218 slots 0/3/6 and the functor vtable 0x15d1dc0 (same four bodies as 0x15d2478)
    require(e.reloc_target(0x1594218) == 0x20B0, "bool class slot 0 = 0x20b0")
    require(e.reloc_target(0x1594218 + 0x18) == 0xB890, "bool class slot 3 = 0xb890 (returns 1)")
    require(e.reloc_target(0x1594218 + 0x30) == 0xB930, "bool class slot 6 = 0xb930 (returns 0)")
    e.bytes_at(0xB890, "b801000000c3", "bool slot 3: mov eax,1; ret")
    e.bytes_at(0xB930, "31c0c3", "bool slot 6: xor eax,eax; ret")
    for i, body in enumerate((0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)):
        require(e.reloc_target(0x15D1DC0 + 8 * i) == body, f"functor vtable 0x15d1dc0 slot {i}")
    e.bytes_at(0xC2F0, "c3", "functor destroy is a bare ret")
    # command class (vtable 0x1594dc0, address point 0x1594dd0) and the invoker cell
    for i, rva in enumerate((0x4B0, 0x22A60, 0x20D60, 0x22AE0)):
        require(e.reloc_target(0x1594DD0 + 8 * i) == rva, f"command vtable slot {i} = {rva:#x}")
    e.bytes_at(0x22AE0, "31c0c3", "command slot 3: xor eax,eax; ret")
    require(e.reloc_target(0x15FE5E0) == 0x1990, "delegate invoker cell 0x15fe5e0 -> 0x1990")
    # the 0x26ea0 default range / slot gate that makes both targets keep {1.0, 10.0}
    t = 0x26EA0
    e.bytes_at(t + 0x89, "c783980000000000803f" "c7839c00000000002041", "0x26ea0 default range {1.0f, 10.0f}")
    e.bytes_at(t + 0xA4, "4c89f7" "498b06" "ff5018" "83f801" "751f" "498b06" "4c89f7" "ff5030" "83f803" "7511",
               "0x26ea0 virtual slot 3 == 1 && slot 6 == 3 gate")
    # object placement is contiguous with the native layout
    require(0x32F8B90 + 0xC8 == 0x32F8C58 and 0x32F8C58 + 0x20 == 0x32F8C78 and 0x32F8C78 + 0xA8 == 0x32F8D20
            and 0x32F8D20 + 0xA8 == 0x32F8DC8 and 0x32F8E08 + 0xA0 == 0x32F8EA8 and 0x32F8EA8 + 0xA0 == 0x32F8F48,
            "object placement")


SPEC = Spec(
    name="slot307", index=307, base=BASE, size=SIZE, sha=SHA, pins=PINS, callees=CALLEES, elf_extra=elf_extra,
    native_files={"header": "native/core/ctor_slot_307_a5b090.hpp", "source": "native/core/ctor_slot_307_a5b090.cpp",
                  "test": "native/tests/test_ctor_slot_307.cpp", "cmake": "native/CMakeLists.txt"},
    header_constants={"kSlot307Rva": BASE, "kSlot307BodySize": SIZE, "kSlot307CellRva": 0x32F8B80,
                      "kSlot307CellDtorRva": 0xA59370, "kSlot307VarRva": 0x32F8B90, "kSlot307FnVtbl": 0x15D1DC0,
                      "kSlot307CaptionRva": 0x32F8C58, "kSlot307GroupRva": 0x32F8C78,
                      "kSlot307Toggle1Rva": 0x32F8D20, "kSlot307Tail1Rva": 0x32F8DC8,
                      "kSlot307Tail1DtorRva": 0xA593A0, "kSlot307Tail2Rva": 0x32F8DE8,
                      "kSlot307Tail2DtorRva": 0xA593B0, "kSlot307Command1Rva": 0x32F8E08,
                      "kSlot307Handler1Rva": 0xA59410, "kSlot307Command2Rva": 0x32F8EA8,
                      "kSlot307Handler2Rva": 0xA595C0, "kSlot307Toggle2Rva": 0x32F8F48,
                      "kSlot307ItemFlags": 0x30},
    header_tokens=['kSlot307VarName[] = "trophyProcessingEnabled"', 'kSlot307Caption[] = "Trophy System"',
                   'kSlot307Parent[] = "Platform Services"', 'kSlot307Toggle1Text[] = "Processing Enabled"',
                   'kSlot307Command1Name[] = "unlockTrophy"', 'kSlot307Command2Name[] = "unlockAllTrophies"',
                   'kSlot307Toggle2Text[] = "Unlock All Trophies"', "struct Slot307Objects"],
    source_order=["kSlot307CellDtorRva, &o.cell",
                  "init_bool_var_static(o.var, name, description, kSlot307FnVtbl, concmd, kSlot307Value, kSlot307Default)",
                  "kSlot307CaptionDtorRva, &o.caption",
                  "init_menu_group_26c10(o.group, parent, o.caption, kSlot307ItemFlags)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.group)",
                  "init_debug_menu_item_26ea0(o.toggle1, o.caption, text1, debug_menu_target_for(o.var)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle1)",
                  "kSlot307Tail1DtorRva, &o.tail1", "kSlot307Tail2DtorRva, &o.tail2",
                  "init_fconcmd_object(o.command1, spec1, concmd)", "init_fconcmd_object(o.command2, spec2, concmd)",
                  "init_debug_menu_item_26ea0(o.toggle2, o.caption, text2, l2::command_menu_target(o.command2)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle2)"],
    test_tokens=["kSlot307Rva == 0xa5b090", "section_build()", "section_failures()", "{0x272d0, &o.toggle2}"],
    cmake_tokens=["core/ctor_slot_307_a5b090.cpp", "add_executable(test_ctor_slot_307 tests/test_ctor_slot_307.cpp)",
                  "add_test(NAME ctor_slot_307 COMMAND test_ctor_slot_307)"],
    summary="trophyProcessingEnabled (0/0, tag 0x15d1dc0) + unlockTrophy/unlockAllTrophies + group + 2 toggles",
)

if __name__ == "__main__":
    raise SystemExit(main(SPEC))
