#!/usr/bin/env python3
"""ELF-tie startup constructor slot 308 (0xa58f10): the "Save System" block of the "Platform Services"
debug menu (sSaveManagerShowInfoLevel ranged int 0..3 through 0xc580, sSaveManagerSaveEnabled bool, group
and two toggles) and the native 0xc580 ranged-int constructor (core/ranged_int_var_c580).

Offline only: opens the pinned private ELF (local/re/eboot.elf), reads bytes, never loads, relocates or
executes an original RVA. Pins: ELF sha256, constructor-table word 308, the 1119-byte body (sha256,
extent, padding), every call / rip operand / immediate of the body, the 0xc580 body by hash and its two
rip operands, the strings, the reused callees by hash, the functor slots 0xc720..0xc770, the int class
0x1594c08 and its slots 3 / 6, the bool class and the native header / source / test / CMake tokens.
Mutation self-test and --tamper elf|native (exits 1) come from l2_slot_check.py.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from l2_slot_check import Spec, main, require  # noqa: E402

BASE = 0xA58F10
SIZE = 1119
SHA = "5f257ebf1dcf612ac4b3740113c2d2cdc7cb329167a9469354efba008d724547"

PINS = [
    ('bytes', 0xa58f1d, '4883e4e0'),
    ('bytes', 0xa58f21, '4881ece0000000'),
    ('rip', 0xa58f28, 7, 3, 0x15fe518),
    ('rip', 0xa58f2f, 7, 3, 0x32f84f8),
    ('rip', 0xa58f36, 7, 3, 0xa50d50),
    ('rip', 0xa58f3d, 7, 3, 0x1604218),
    ('bytes', 0xa58f4f, '48c70600000000'),
    ('call', 0xa58f56, 0xf8f390),
    ('rip', 0xa58f5b, 7, 3, 0x1594bf8),
    ('rip', 0xa58f62, 7, 3, 0x12c40c0),
    ('rip', 0xa58f6d, 7, 3, 0x32f8508),
    ('bytes', 0xa58f79, 'c744243c00000000'),
    ('bytes', 0xa58f81, 'c744243803000000'),
    ('bytes', 0xa58f91, '48c784249000000000000000'),
    ('rip', 0xa58fa2, 7, 3, 0x32f85d8),
    ('bytes', 0xa58fb3, '41b901000000'),
    ('rip', 0xa58fbe, 7, 3, 0x12c40da),
    ('bytes', 0xa58fc5, '48c744244819000000'),
    ('bytes', 0xa58fdf, '48c744242028000000'),
    ('rip', 0xa58ff2, 7, 3, 0x15d1d80),
    ('bytes', 0xa59007, '48c78424a800000000000000'),
    ('bytes', 0xa59018, '4883c310'),
    ('rip', 0xa5901c, 7, 3, 0x32f8508),
    ('call', 0xa59026, 0xc580),
    ('bytes', 0xa5902b, '4883c410'),
    ('rip', 0xa59040, 7, 3, 0x32f8508),
    ('rip', 0xa59047, 10, 2, 0x32f85d8),
    ('bytes', 0xa5904d, '00000000'),
    ('rip', 0xa59051, 10, 2, 0x32f8570),
    ('bytes', 0xa59057, '00000000'),
    ('call', 0xa5905b, 0x20d60),
    ('call', 0xa59063, 0xf590),
    ('call', 0xa5906b, 0xf590),
    ('rip', 0xa59070, 7, 3, 0x18a60),
    ('rip', 0xa59077, 7, 3, 0x1604218),
    ('call', 0xa59084, 0xf8f390),
    ('rip', 0xa59089, 7, 3, 0x1594208),
    ('rip', 0xa59090, 7, 3, 0x12c4103),
    ('rip', 0xa59097, 7, 3, 0x32f85e0),
    ('bytes', 0xa590ac, 'b901000000'),
    ('rip', 0xa590b6, 7, 3, 0x12c41fe),
    ('bytes', 0xa590c6, '48c744244817000000'),
    ('bytes', 0xa590e0, '48c744247800000000'),
    ('bytes', 0xa590e9, '4883c310'),
    ('rip', 0xa590ed, 7, 3, 0x32f85e0),
    ('call', 0xa590f4, 0x20af0),
    ('rip', 0xa590f9, 7, 3, 0x15940a0),
    ('rip', 0xa59103, 7, 3, 0x32f86a0),
    ('rip', 0xa5910e, 7, 3, 0x15d1da0),
    ('bytes', 0xa59115, '4883c010'),
    ('rip', 0xa59119, 7, 3, 0x32f85e0),
    ('rip', 0xa59120, 7, 3, 0x32f8640),
    ('rip', 0xa59127, 10, 2, 0x32f8649),
    ('bytes', 0xa5912d, '00000000'),
    ('rip', 0xa59131, 8, 4, 0x32f8650),
    ('rip', 0xa59139, 11, 3, 0x32f8680),
    ('bytes', 0xa59140, '00000000'),
    ('rip', 0xa59144, 7, 3, 0x32f8690),
    ('rip', 0xa5914b, 11, 3, 0x32f8698),
    ('bytes', 0xa59152, '00000000'),
    ('call', 0xa59156, 0xf590),
    ('rip', 0xa5915b, 7, 3, 0x32f85e0),
    ('rip', 0xa59165, 7, 2, 0x32f86a0),
    ('bytes', 0xa5916b, '01'),
    ('rip', 0xa5916c, 7, 2, 0x32f8648),
    ('bytes', 0xa59172, '01'),
    ('call', 0xa59176, 0x20d60),
    ('call', 0xa5917e, 0xf590),
    ('rip', 0xa59183, 7, 3, 0x20b0),
    ('call', 0xa59190, 0xf8f390),
    ('rip', 0xa59195, 7, 3, 0x32f86a8),
    ('rip', 0xa591a6, 7, 3, 0x12c411b),
    ('rip', 0xa591ad, 7, 3, 0x12c412d),
    ('bytes', 0xa591b8, 'b930000000'),
    ('bytes', 0xa591c8, '48c744246811000000'),
    ('bytes', 0xa591d6, '48c74424480b000000'),
    ('call', 0xa591ee, 0x26c10),
    ('call', 0xa591f6, 0xf590),
    ('call', 0xa591fe, 0xf590),
    ('rip', 0xa59203, 7, 3, 0x272d0),
    ('rip', 0xa5920a, 7, 3, 0x1604218),
    ('call', 0xa5921a, 0xf8f390),
    ('rip', 0xa5921f, 7, 3, 0x32f8750),
    ('rip', 0xa59235, 7, 3, 0x12c4139),
    ('bytes', 0xa59240, '41b830000000'),
    ('rip', 0xa59246, 7, 3, 0x32f8508),
    ('bytes', 0xa5924d, '48c74424680b000000'),
    ('bytes', 0xa59261, '48c74424480f000000'),
    ('call', 0xa59279, 0x26ea0),
    ('call', 0xa59281, 0xf590),
    ('call', 0xa59289, 0xf590),
    ('call', 0xa59297, 0xf8f390),
    ('rip', 0xa5929c, 7, 3, 0x32f87f8),
    ('rip', 0xa592a3, 7, 3, 0x12c412d),
    ('bytes', 0xa592b8, '41b830000000'),
    ('rip', 0xa592be, 7, 3, 0x32f85e0),
    ('rip', 0xa592ca, 7, 3, 0x12c4149),
    ('bytes', 0xa592d7, '48c74424680b000000'),
    ('bytes', 0xa592eb, '48c744244812000000'),
    ('call', 0xa592fd, 0x26ea0),
    ('call', 0xa59305, 0xf590),
    ('call', 0xa5930d, 0xf590),
    ('call', 0xa5931b, 0xf8f390),
    ('rip', 0xa59320, 7, 3, 0x32f88a8),
    ('rip', 0xa59327, 7, 3, 0xa55f30),
    ('bytes', 0xa5933a, '48c7461800000000'),
    ('call', 0xa59342, 0xf8f390),
    ('rip', 0xa59347, 7, 3, 0x15fe518),
    ('bytes', 0xa59359, '750f'),
    ('bytes', 0xa59369, 'c3'),
    ('call', 0xa5936a, 0xf8f3a0),
]

CALLEES = {
    "concmd_base_ctor": (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45"),
    "concmd_register": (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "bool_dtor": (0x20B0, 0x36, "73f646e91ca2f7d8dd14cf1858c93197eddbf00a4267f6ca36ea00b2500efe42"),
    "menu_group_ctor": (0x26C10, 0xC2, "cc1c2bcce3014d52d61f88a2ac0b3c6a0666198cba7527d46d3e2ebaf03114fe"),
    "menu_toggle_ctor": (0x26EA0, 0xED, "c2c7d8c17db68783d9e94100c4f032f3bf06c1ac7d12d5f48e0e34e06a924677"),
    "menu_item_dtor": (0x272D0, 0x19E, "41ea4aa9e998a114e13cd7149228ae5c542debbeb084c1d412a988f2e801c770"),
    "bool_slot3": (0xB890, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "bool_slot6": (0xB930, 0x3, "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf"),
    "ranged_int_ctor": (0xC580, 0x15F, "a93bc0f34cfd49fbbcbc7f2186b2668de6e49f5f54012caafca7f17dfb396bce"),
    "fn_copy": (0xC720, 0x14, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
    "fn_clone": (0xC740, 0x14, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
    "fn_move": (0xC760, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "fn_destroy": (0xC770, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "int_dtor": (0x18A60, 0x36, "4de4f3993b02c84c9c20824690e8ab9d4f7a1b84babfa6cbe0d89c9dd558b50b"),
    "int_slot3": (0xBF70, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "int_slot6": (0xBFC0, 0x6, "7140f35dee6220b79b12aecc27acf5105bf3b77d1588e89fce345de7c16c72b7"),
}


def elf_extra(e) -> None:
    for addr, text in ((0x12C40C0, b"sSaveManagerShowInfoLevel"),
                       (0x12C40DA, b"Save System Debug Info Level from 0 to 3"),
                       (0x12C4103, b"sSaveManagerSaveEnabled"), (0x12C411B, b"Platform Services"),
                       (0x12C412D, b"Save System"), (0x12C4139, b"Show Info Level"),
                       (0x12C4149, b"Enable Save System")):
        e.cstring(addr, text)
    require(e.read(0x12C41FE, 1) == b"\0", "empty description points at a NUL byte")
    # 0xc580: mid vptr table 0x1594190 (+0x10), functor class 0x15944a0 (rip operands of the constructor)
    require(e.rip(0xC5BB, 7, 3) == 0x1594190, "0xc580 mid vtable 0x1594190")
    require(e.rip(0xC5C6, 7, 3) == 0x15944A0, "0xc580 functor class 0x15944a0")
    e.bytes_at(0xC5CD, "4883c010", "0xc580: mid vptr = table + 0x10")
    # the caller functor tag 0x15d1d80 and the class tag 0x15944a0 share the four slot bodies
    for base in (0x15D1D80, 0x15944A0):
        for i, body in enumerate((0xC720, 0xC740, 0xC760, 0xC770)):
            require(e.reloc_target(base + 8 * i) == body, f"functor vtable {base:#x} slot {i}")
    e.bytes_at(0xC770, "c3", "functor destroy is a bare ret")
    require(e.rip(0xC724, 7, 3) == 0x15944A0, "functor copy stores the class tag 0x15944a0")
    require(e.rip(0xC740, 7, 3) == 0x15944A0, "functor clone stores the class tag 0x15944a0")
    # final int class: table 0x1594bf8 (+0x10 = 0x1594c08): slot 0 = dtor 0x18a60, 2 = register, 3 = 1, 6 = 2
    require(e.reloc_target(0x1594C08) == 0x18A60, "int class slot 0 = 0x18a60")
    require(e.reloc_target(0x1594C08 + 0x10) == 0x20D60, "int class slot 2 = 0x20d60")
    require(e.reloc_target(0x1594C08 + 0x18) == 0xBF70, "int class slot 3 = 0xbf70 (returns 1)")
    require(e.reloc_target(0x1594C08 + 0x30) == 0xBFC0, "int class slot 6 = 0xbfc0 (returns 2)")
    e.bytes_at(0xBF70, "b801000000c3", "int slot 3: mov eax,1; ret")
    e.bytes_at(0xBFC0, "b802000000c3", "int slot 6: mov eax,2; ret")
    # bool class 0x1594218 (final) slots 0/3/6 and the bool functor vtable 0x15d1da0 (same four bodies)
    require(e.reloc_target(0x1594218) == 0x20B0, "bool class slot 0 = 0x20b0")
    require(e.reloc_target(0x1594218 + 0x18) == 0xB890, "bool class slot 3 = 0xb890 (returns 1)")
    require(e.reloc_target(0x1594218 + 0x30) == 0xB930, "bool class slot 6 = 0xb930 (returns 0)")
    e.bytes_at(0xB890, "b801000000c3", "bool slot 3: mov eax,1; ret")
    e.bytes_at(0xB930, "31c0c3", "bool slot 6: xor eax,eax; ret")
    for i, body in enumerate((0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)):
        require(e.reloc_target(0x15D1DA0 + 8 * i) == body, f"bool functor vtable 0x15d1da0 slot {i}")
    # the 0x26ea0 default range / slot gate: slot 3 == 1 and slot 6 == 3 narrows; the int class gives 1 and 2
    t = 0x26EA0
    e.bytes_at(t + 0x89, "c783980000000000803f" "c7839c00000000002041", "0x26ea0 default range {1.0f, 10.0f}")
    e.bytes_at(t + 0xA4, "4c89f7" "498b06" "ff5018" "83f801" "751f" "498b06" "4c89f7" "ff5030" "83f803" "7511",
               "0x26ea0 virtual slot 3 == 1 && slot 6 == 3 gate")
    # object placement is contiguous with the native layout
    require(0x32F8508 + 0xD8 == 0x32F85E0 and 0x32F85E0 + 0xC8 == 0x32F86A8 and 0x32F86A8 + 0xA8 == 0x32F8750
            and 0x32F8750 + 0xA8 == 0x32F87F8, "object placement")
    require(0x32F8508 + 0xD0 == 0x32F85D8 and 0x32F85E0 + 0xC0 == 0x32F86A0 and 0x32F8508 + 0x68 == 0x32F8570,
            "value cells / default cell")


SPEC = Spec(
    name="slot308", index=308, base=BASE, size=SIZE, sha=SHA, pins=PINS, callees=CALLEES, elf_extra=elf_extra,
    native_files={"header": "native/core/ctor_slot_308_a58f10.hpp", "source": "native/core/ctor_slot_308_a58f10.cpp",
                  "test": "native/tests/test_ctor_slot_308.cpp", "cmake": "native/CMakeLists.txt"},
    header_constants={"kSlot308Rva": BASE, "kSlot308BodySize": SIZE, "kSlot308CellRva": 0x32F84F8,
                      "kSlot308CellDtorRva": 0xA50D50, "kSlot308LevelRva": 0x32F8508,
                      "kSlot308EnabledRva": 0x32F85E0, "kSlot308GroupRva": 0x32F86A8,
                      "kSlot308Toggle1Rva": 0x32F8750, "kSlot308Toggle2Rva": 0x32F87F8,
                      "kSlot308TailRva": 0x32F88A8, "kSlot308TailDtorRva": 0xA55F30,
                      "kSlot308ArgFnVtbl": 0x15D1D80, "kSlot308BoolFnVtbl": 0x15D1DA0, "kSlot308LevelMax": 3,
                      "kSlot308LevelFlags": 1, "kSlot308EnabledValue": 1, "kSlot308EnabledDefault": 1,
                      "kSlot308ItemFlags": 0x30},
    header_tokens=['kSlot308LevelName[] = "sSaveManagerShowInfoLevel"',
                   'kSlot308LevelDesc[] = "Save System Debug Info Level from 0 to 3"',
                   'kSlot308EnabledName[] = "sSaveManagerSaveEnabled"', 'kSlot308Parent[] = "Platform Services"',
                   'kSlot308Caption[] = "Save System"', 'kSlot308Toggle1Text[] = "Show Info Level"',
                   'kSlot308Toggle2Text[] = "Enable Save System"', "struct Slot308Objects"],
    source_order=["kSlot308CellDtorRva, &o.cell",
                  "init_ranged_int_var_c580(o.level, level_name, &o.level.value, kSlot308LevelMin, kSlot308LevelMax",
                  "o.level.base.vptr = kRangedIntFinalVptr",
                  "register_concmd_20d60(o.level.base, concmd)",
                  "record(kRangedIntDtorRva, &o.level)",
                  "init_bool_var_static(o.enabled, enabled_name, enabled_desc, kSlot308BoolFnVtbl",
                  "init_menu_group_26c10(o.group, parent, caption, kSlot308ItemFlags)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.group)",
                  "init_debug_menu_item_26ea0(o.toggle1, label1, text1, ranged_int_c580_menu_target(o.level)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle1)",
                  "init_debug_menu_item_26ea0(o.toggle2, label2, text2, debug_menu_target_for(o.enabled)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle2)",
                  "kSlot308TailDtorRva, &o.tail"],
    test_tokens=["kSlot308Rva == 0xa58f10", "section_build()", "section_failures()", "{0xa55f30, &o.tail}"],
    cmake_tokens=["core/ctor_slot_308_a58f10.cpp", "core/ranged_int_var_c580.cpp",
                  "add_executable(test_ctor_slot_308 tests/test_ctor_slot_308.cpp)",
                  "add_test(NAME ctor_slot_308 COMMAND test_ctor_slot_308)"],
    summary="sSaveManagerShowInfoLevel (int 0..3 via 0xc580) + sSaveManagerSaveEnabled (bool 1/1) + group + 2 toggles",
)

if __name__ == "__main__":
    raise SystemExit(main(SPEC))
