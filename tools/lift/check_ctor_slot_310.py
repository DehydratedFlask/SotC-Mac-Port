#!/usr/bin/env python3
"""ELF-tie startup constructor slot 310 (0xa479d0): the `titleid` string variable (0x257a0 / 0x258f0), the
`useCircleAsEnter` bool variable, two "TitleInfo" toggles and the null tail pointer.

Offline only: opens the pinned private ELF (local/re/eboot.elf), reads bytes, never loads, relocates or
executes an original RVA. Pins: ELF sha256, constructor-table word 310, the 723-byte body (sha256, extent,
padding), every call / rip operand / immediate of the body, the strings, the reused callees by hash (the
string-variable constructors, the functor class 0x1595068 slots, the class slots 3 / 6, the shared menu and
console constructors), the vtable relocations and the native header / source / test / CMake tokens.
Mutation self-test and --tamper elf|native (exits 1) come from l2_slot_check.py.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from l2_slot_check import Spec, main, require  # noqa: E402

BASE = 0xA479D0
SIZE = 723
SHA = "eca5ec3005f7e22cd0eac03bb022bec863fbed1d01eb863688b3cd037699fa03"

PINS = [
    ('bytes', 0xa479dd, '4883ec48'),
    ('rip', 0xa479e1, 7, 3, 0x15fe518),
    ('rip', 0xa479f0, 7, 3, 0x32f7648),
    ('rip', 0xa479f7, 7, 3, 0x12c30e0),
    ('rip', 0xa47a02, 7, 3, 0x12c30e8),
    ('bytes', 0xa47a1d, '48c745b807000000'),
    ('bytes', 0xa47a33, '48c745a800000000'),
    ('call', 0xa47a3b, 0x257a0),
    ('call', 0xa47a43, 0xf590),
    ('call', 0xa47a4b, 0xf590),
    ('rip', 0xa47a50, 7, 3, 0x23c90),
    ('rip', 0xa47a57, 7, 3, 0x1604218),
    ('call', 0xa47a61, 0xf8f390),
    ('rip', 0xa47a66, 7, 3, 0x1594208),
    ('rip', 0xa47a6d, 7, 3, 0x32f7790),
    ('rip', 0xa47a7c, 7, 3, 0x12c30e9),
    ('bytes', 0xa47a87, 'b901000000'),
    ('bytes', 0xa47a9d, '48c7459810000000'),
    ('bytes', 0xa47aaf, '48c745c800000000'),
    ('bytes', 0xa47ab7, '4883c310'),
    ('rip', 0xa47abb, 7, 3, 0x32f7790),
    ('call', 0xa47ac2, 0x20af0),
    ('rip', 0xa47ac7, 7, 3, 0x15940a0),
    ('rip', 0xa47ad1, 7, 3, 0x32f7850),
    ('rip', 0xa47adc, 7, 3, 0x15d19b0),
    ('bytes', 0xa47ae3, '4883c010'),
    ('rip', 0xa47ae7, 7, 3, 0x32f7790),
    ('rip', 0xa47aee, 7, 3, 0x32f77f0),
    ('rip', 0xa47af5, 10, 2, 0x32f77f9),
    ('bytes', 0xa47afb, '00000000'),
    ('rip', 0xa47aff, 8, 4, 0x32f7800),
    ('rip', 0xa47b07, 11, 3, 0x32f7830),
    ('bytes', 0xa47b0e, '00000000'),
    ('rip', 0xa47b12, 7, 3, 0x32f7840),
    ('rip', 0xa47b19, 11, 3, 0x32f7848),
    ('bytes', 0xa47b20, '00000000'),
    ('call', 0xa47b24, 0xf590),
    ('rip', 0xa47b29, 7, 3, 0x32f7790),
    ('rip', 0xa47b33, 7, 2, 0x32f7850),
    ('bytes', 0xa47b39, '00'),
    ('rip', 0xa47b3a, 7, 2, 0x32f77f8),
    ('bytes', 0xa47b40, '00'),
    ('call', 0xa47b44, 0x20d60),
    ('call', 0xa47b4c, 0xf590),
    ('rip', 0xa47b51, 7, 3, 0x1604218),
    ('rip', 0xa47b58, 7, 3, 0x20b0),
    ('call', 0xa47b65, 0xf8f390),
    ('rip', 0xa47b6a, 7, 3, 0x32f7858),
    ('rip', 0xa47b79, 7, 3, 0x12c30fa),
    ('rip', 0xa47b80, 7, 3, 0x12c3104),
    ('bytes', 0xa47b8b, '41b830000000'),
    ('rip', 0xa47b91, 7, 3, 0x32f7648),
    ('bytes', 0xa47ba5, '48c745b809000000'),
    ('bytes', 0xa47bb1, '48c7459808000000'),
    ('call', 0xa47bc3, 0x26ea0),
    ('call', 0xa47bcb, 0xf590),
    ('call', 0xa47bd3, 0xf590),
    ('rip', 0xa47bd8, 7, 3, 0x272d0),
    ('call', 0xa47be8, 0xf8f390),
    ('rip', 0xa47bf1, 7, 3, 0x32f7900),
    ('rip', 0xa47c00, 7, 3, 0x12c310d),
    ('bytes', 0xa47c0b, '41b830000000'),
    ('rip', 0xa47c11, 7, 3, 0x32f7790),
    ('bytes', 0xa47c18, '48c745b809000000'),
    ('bytes', 0xa47c2d, '48c745980f000000'),
    ('call', 0xa47c3f, 0x26ea0),
    ('call', 0xa47c47, 0xf590),
    ('call', 0xa47c4f, 0xf590),
    ('call', 0xa47c5d, 0xf8f390),
    ('rip', 0xa47c62, 7, 3, 0x32f79a8),
    ('rip', 0xa47c69, 7, 3, 0xa46ba0),
    ('bytes', 0xa47c73, '48c70600000000'),
    ('call', 0xa47c7a, 0xf8f390),
    ('rip', 0xa47c7f, 7, 3, 0x15fe518),
    ('bytes', 0xa47c8d, '750f'),
    ('bytes', 0xa47c8f, '4883c448'),
    ('bytes', 0xa47c9d, 'c3'),
    ('call', 0xa47c9e, 0xf8f3a0),
]

CALLEES = {
    "concmd_base_ctor": (0x20AF0, 0x5a, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45"),
    "concmd_register": (0x20D60, 0x1a, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "bool_dtor": (0x20B0, 0x36, "73f646e91ca2f7d8dd14cf1858c93197eddbf00a4267f6ca36ea00b2500efe42"),
    "menu_toggle_ctor": (0x26EA0, 0xed, "c2c7d8c17db68783d9e94100c4f032f3bf06c1ac7d12d5f48e0e34e06a924677"),
    "menu_item_dtor": (0x272D0, 0x19e, "41ea4aa9e998a114e13cd7149228ae5c542debbeb084c1d412a988f2e801c770"),
    "string_ctor": (0x257A0, 0x14c, "9ac816cf6bebd628d2a9b2941571869d4f1d08994081f5c61db7d165270115f2"),
    "string_inner": (0x258F0, 0x157, "5555aff241162d36f3c442755b6342ac872e92bbdf296839033b3a2b086d6950"),
    "empty_ctor_f180": (0xF180, 0x1c, "bd33279f6055c44aae01e0f9cdcb392acdadfcf4749dc6fe6e6113f5940f714c"),
    "sfn_copy": (0x25A50, 0x14, "73be203a7605ec0af252ac3df43ce59dc067315828552214dff5a1160c06fb6e"),
    "sfn_clone": (0x25A70, 0x14, "5400d3e5266832c0e4cde2131420856e018aeac1fa816f68d2e5487d98496897"),
    "sfn_move": (0x25A90, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "sfn_destroy": (0x25AA0, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "str_slot3": (0x246A0, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "str_slot6": (0x24780, 0x6, "e36623525b7c35b277173e56616b2d865cf6515aa2be1d12d6814ef725cecffb"),
    "str_dtor_mid": (0x24610, 0x28, "60ae896a5c15d364a5e59c48c944d2c75d2e61a9a795ab73ae8f47299005fcc6"),
    "bfn_copy": (0xC2A0, 0x14, "b355d74a6edc25ac52cd52ef1de89719ec412b8ea6414e806f80cea7a60e5693"),
    "bfn_clone": (0xC2C0, 0x14, "047e110aeff5fe3000666c334afa7c4e6c799a779125a7608c000e1eed1a7474"),
    "bfn_move": (0xC2E0, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "bfn_destroy": (0xC2F0, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "tail_dtor": (0xA46BA0, 0xcd, "2134e8fbce41f9e01f11db153534b9ac5b98ac1d775a1e2e1ccefbbbc2c4d741"),
}


def elf_extra(e) -> None:
    for addr, text in ((0x12C30E0, b"titleid"), (0x12C30E9, b"useCircleAsEnter"), (0x12C30FA, b"TitleInfo"),
                       (0x12C3104, b"Title ID"), (0x12C310D, b"Circle as Enter")):
        e.cstring(addr, text)
    require(e.read(0x12C30E8, 1) == b"\0", "empty view points at a NUL byte")
    require(e.read(0xF96388, 1) == b"\0", "string-var description view points at a NUL byte")
    # string class 0x1594ee0 (+0x10 = 0x1594ef0): slot 0 = dtor 0x23c90, 2 = register, 3 = 1, 6 = 4
    require(e.reloc_target(0x1594EF0) == 0x23C90, "string class slot 0 = 0x23c90")
    require(e.reloc_target(0x1594EF0 + 0x10) == 0x20D60, "string class slot 2 = 0x20d60")
    require(e.reloc_target(0x1594EF0 + 0x18) == 0x246A0, "string class slot 3 = 0x246a0 (returns 1)")
    require(e.reloc_target(0x1594EF0 + 0x30) == 0x24780, "string class slot 6 = 0x24780 (returns 4)")
    e.bytes_at(0x246A0, "b801000000c3", "string slot 3: mov eax,1; ret")
    e.bytes_at(0x24780, "b804000000c3", "string slot 6: mov eax,4; ret")
    # functor class 0x1595068: copy / clone / move / destroy
    for i, body in enumerate((0x25A50, 0x25A70, 0x25A90, 0x25AA0)):
        require(e.reloc_target(0x1595068 + 8 * i) == body, f"functor class 0x1595068 slot {i}")
    require(e.rip(0x25A54, 7, 3) == 0x1595068 and e.rip(0x25A70, 7, 3) == 0x1595068, "functor stores its class tag")
    # the bool functor tag 0x15d19b0 has the same four first slot bodies as the accepted tags
    for i, body in enumerate((0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)):
        require(e.reloc_target(0x15D19B0 + 8 * i) == body, f"bool functor 0x15d19b0 slot {i}")
    # inner / outer vptr stores of the string constructors
    require(e.rip(0x257BF, 7, 3) == 0x1594EE0 and e.rip(0x25921, 7, 3) == 0x1594F58, "string constructor vtables")
    # the 0x26ea0 default range / slot gate (slot 3 == 1 && slot 6 == 3 narrows; 1 and 4 do not)
    t = 0x26EA0
    e.bytes_at(t + 0x89, "c783980000000000803f" "c7839c00000000002041", "0x26ea0 default range {1.0f, 10.0f}")
    e.bytes_at(t + 0xA4, "4c89f7" "498b06" "ff5018" "83f801" "751f" "498b06" "4c89f7" "ff5030" "83f803" "7511",
               "0x26ea0 virtual slot 3 == 1 && slot 6 == 3 gate")
    # the tail pointer's destructor 0xa46ba0 and the object placement
    require(0x32F7648 + 0x148 == 0x32F7790 and 0x32F7790 + 0xC8 == 0x32F7858 and 0x32F7858 + 0xA8 == 0x32F7900
            and 0x32F7900 + 0xA8 == 0x32F79A8, "object placement")
    require(0x32F7648 + 0x128 == 0x32F7770 and 0x32F7790 + 0xC0 == 0x32F7850, "value cells")


SPEC = Spec(
    name="slot310", index=310, base=BASE, size=SIZE, sha=SHA, pins=PINS, callees=CALLEES, elf_extra=elf_extra,
    native_files={"header": "native/core/ctor_slot_310_a479d0.hpp", "source": "native/core/ctor_slot_310_a479d0.cpp",
                  "test": "native/tests/test_ctor_slot_310.cpp", "cmake": "native/CMakeLists.txt",
                  "strhdr": "native/core/string_var_257a0.hpp", "strsrc": "native/core/string_var_257a0.cpp"},
    header_constants={"kSlot310Rva": BASE, "kSlot310BodySize": SIZE, "kSlot310TitleIdRva": 0x32F7648,
                      "kSlot310CircleRva": 0x32F7790, "kSlot310ToggleTitleRva": 0x32F7858,
                      "kSlot310ToggleCircleRva": 0x32F7900, "kSlot310TailRva": 0x32F79A8,
                      "kSlot310TailDtorRva": 0xA46BA0, "kSlot310CircleFnVtbl": 0x15D19B0, "kSlot310ItemFlags": 0x30},
    header_tokens=['kSlot310TitleIdName[] = "titleid"', 'kSlot310CircleName[] = "useCircleAsEnter"',
                   'kSlot310Label[] = "TitleInfo"', 'kSlot310TitleText[] = "Title ID"',
                   'kSlot310CircleText[] = "Circle as Enter"', "struct Slot310Objects"],
    source_order=["init_string_var_257a0(o.titleid, name, empty, concmd)",
                  "record(kStringVarDtorRva, &o.titleid)",
                  "init_bool_var_static(o.circle", "init_debug_menu_item_26ea0(o.toggle_title",
                  "string_var_menu_target(o.titleid)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle_title)",
                  "init_debug_menu_item_26ea0(o.toggle_circle", "debug_menu_target_for(o.circle)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle_circle)",
                  "record(kSlot310TailDtorRva, &o.tail)"],
    test_tokens=["kSlot310Rva == 0xa479d0", "section_string_var_inner()", "{0xa46ba0, &o.tail}",
                 "counter == 3", "kStringVarFnVtbl"],
    cmake_tokens=["core/string_var_257a0.cpp", "core/ctor_slot_310_a479d0.cpp",
                  "add_executable(test_ctor_slot_310 tests/test_ctor_slot_310.cpp)",
                  "add_test(NAME ctor_slot_310 COMMAND test_ctor_slot_310)"],
    summary="titleid string var (0x257a0/0x258f0) + useCircleAsEnter bool var + 2 TitleInfo toggles + tail",
)

if __name__ == "__main__":
    raise SystemExit(main(SPEC))
