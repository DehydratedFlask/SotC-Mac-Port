#!/usr/bin/env python3
"""ELF-tie startup constructor slot 317 (0xa2c390): the "Controller" block of the "Platform Services" debug
menu (vibration_large_scalar / vibration_small_scalar unbounded floats through 0x6aa00, a group and two
toggles whose range is narrowed to {0.1, 0.5} by the float class).

Offline only: opens the pinned private ELF (local/re/eboot.elf), reads bytes, never loads, relocates or
executes an original RVA. Pins: ELF sha256, constructor-table word 317, the 969-byte body (sha256, extent,
padding), every call / rip operand / immediate of the body, the strings, the reused callees by hash
(including 0x6aa00), the functor vtable 0x15d10f0 (same four bodies as the accepted tag 0x15d57e0), the
float class 0x15942b0 and its slots 3 / 6, and the native header / source / test / CMake tokens.
Mutation self-test and --tamper elf|native (exits 1) come from l2_slot_check.py.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from l2_slot_check import Spec, main, require  # noqa: E402

BASE = 0xA2C390
SIZE = 969
SHA = "83ca4ca75841ea72a4227e58deea703feffb153f140e44ea143e8959656220bd"

PINS = [
    ('bytes', 0xa2c39d, '4883e4e0'),
    ('bytes', 0xa2c3a1, '4881ecc0000000'),
    ('rip', 0xa2c3a8, 7, 3, 0x15fe518),
    ('rip', 0xa2c3af, 7, 3, 0x15942a0),
    ('rip', 0xa2c3b6, 7, 3, 0x32f4c38),
    ('rip', 0xa2c3c2, 7, 3, 0x12c1520),
    ('rip', 0xa2c3cd, 7, 3, 0x15d10f0),
    ('rip', 0xa2c3dc, 7, 3, 0x12c159e),
    ('bytes', 0xa2c3e8, 'b901000000'),
    ('bytes', 0xa2c3f3, '4983c710'),
    ('bytes', 0xa2c406, '48c744247000000000'),
    ('bytes', 0xa2c427, '48c744240816000000'),
    ('bytes', 0xa2c445, '48c784248800000000000000'),
    ('bytes', 0xa2c451, '48c744243800000000'),
    ('call', 0xa2c45a, 0x6aa00),
    ('call', 0xa2c462, 0xf590),
    ('bytes', 0xa2c47e, '41c785d00000000000003f'),
    ('bytes', 0xa2c489, '41c745680000003f'),
    ('call', 0xa2c491, 0x20d60),
    ('call', 0xa2c499, 0xf590),
    ('rip', 0xa2c49e, 7, 3, 0x20f0),
    ('rip', 0xa2c4a5, 7, 3, 0x1604218),
    ('call', 0xa2c4af, 0xf8f390),
    ('rip', 0xa2c4b4, 7, 3, 0x32f4d10),
    ('rip', 0xa2c4bb, 7, 3, 0x12c1537),
    ('bytes', 0xa2c4dd, 'b901000000'),
    ('bytes', 0xa2c4e2, '48c744247000000000'),
    ('rip', 0xa2c4f4, 7, 3, 0x15d10f0),
    ('bytes', 0xa2c501, '48c744240816000000'),
    ('bytes', 0xa2c51c, '48c744243800000000'),
    ('bytes', 0xa2c52d, '48c784248800000000000000'),
    ('call', 0xa2c547, 0x6aa00),
    ('call', 0xa2c54f, 0xf590),
    ('bytes', 0xa2c569, '41c785d00000000000003f'),
    ('bytes', 0xa2c574, '41c745680000003f'),
    ('call', 0xa2c57c, 0x20d60),
    ('call', 0xa2c584, 0xf590),
    ('rip', 0xa2c589, 7, 3, 0x1604218),
    ('rip', 0xa2c590, 7, 3, 0x20f0),
    ('call', 0xa2c59d, 0xf8f390),
    ('rip', 0xa2c5a2, 7, 3, 0x12c1560),
    ('rip', 0xa2c5a9, 7, 3, 0x32f4de8),
    ('rip', 0xa2c5ba, 7, 3, 0x12c154e),
    ('bytes', 0xa2c5c5, 'b930000000'),
    ('bytes', 0xa2c5dd, '48c744244811000000'),
    ('bytes', 0xa2c5e6, '48c74424280a000000'),
    ('call', 0xa2c5fb, 0x26c10),
    ('call', 0xa2c603, 0xf590),
    ('call', 0xa2c60b, 0xf590),
    ('rip', 0xa2c610, 7, 3, 0x272d0),
    ('call', 0xa2c61d, 0xf8f390),
    ('rip', 0xa2c622, 7, 3, 0x32f4c38),
    ('rip', 0xa2c629, 7, 3, 0x12c1560),
    ('rip', 0xa2c630, 7, 3, 0x32f4e90),
    ('bytes', 0xa2c645, '41b830000000'),
    ('rip', 0xa2c650, 7, 3, 0x12c156b),
    ('bytes', 0xa2c660, '48c74424480a000000'),
    ('bytes', 0xa2c674, '48c744242816000000'),
    ('call', 0xa2c683, 0x26ea0),
    ('call', 0xa2c68b, 0xf590),
    ('call', 0xa2c693, 0xf590),
    ('rip', 0xa2c698, 7, 3, 0x272d0),
    ('call', 0xa2c6a8, 0xf8f390),
    ('rip', 0xa2c6ad, 7, 3, 0x12c1582),
    ('rip', 0xa2c6b4, 7, 3, 0x32f4f38),
    ('rip', 0xa2c6c5, 7, 3, 0x12c1560),
    ('bytes', 0xa2c6d0, '41b830000000'),
    ('bytes', 0xa2c6ec, '48c74424480a000000'),
    ('bytes', 0xa2c6f5, '48c744242816000000'),
    ('call', 0xa2c70a, 0x26ea0),
    ('call', 0xa2c712, 0xf590),
    ('call', 0xa2c71a, 0xf590),
    ('rip', 0xa2c71f, 7, 3, 0x272d0),
    ('call', 0xa2c72c, 0xf8f390),
    ('rip', 0xa2c731, 7, 3, 0x15fe518),
    ('bytes', 0xa2c743, '750f'),
    ('bytes', 0xa2c753, 'c3'),
    ('call', 0xa2c754, 0xf8f3a0),
]

CALLEES = {
    "concmd_base_ctor": (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45"),
    "concmd_register": (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "menu_group_ctor": (0x26C10, 0xC2, "cc1c2bcce3014d52d61f88a2ac0b3c6a0666198cba7527d46d3e2ebaf03114fe"),
    "menu_toggle_ctor": (0x26EA0, 0xED, "c2c7d8c17db68783d9e94100c4f032f3bf06c1ac7d12d5f48e0e34e06a924677"),
    "menu_item_dtor": (0x272D0, 0x19E, "41ea4aa9e998a114e13cd7149228ae5c542debbeb084c1d412a988f2e801c770"),
    "unbounded_float_ctor": (0x6AA00, 0x142, "ee90dea5d0e2ed59ce83ff52c5a3732419635622c7b422f3eff54fd1f5384ebb"),
    "fn_copy": (0xC4A0, 0x14, "0137c6226a7dcb380747d73f16a87fab37c63e9c2b8f67d0d1a8f875f2d44a08"),
    "fn_clone": (0xC4C0, 0x14, "5f93b13fd6577d64a7242762148d8d994a40ebd9480009297cce40f30caea597"),
    "fn_move": (0xC4E0, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "fn_destroy": (0xC4F0, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "float_dtor": (0x20F0, 0x36, "e93fb896a676df342779b680fc43ca88aa08aaab78eece0b260f1b6b93df5579"),
    "float_slot3": (0xBBA0, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "float_slot6": (0xBBF0, 0x6, "7adb2a5467d3718ab3b55c6851599af573d4fbeadcd90dd961cfd69d4f94cc9f"),
}


def elf_extra(e) -> None:
    for addr, text in ((0x12C1520, b"vibration_large_scalar"), (0x12C1537, b"vibration_small_scalar"),
                       (0x12C154E, b"Platform Services"), (0x12C1560, b"Controller"),
                       (0x12C156B, b"Large Vibration Scalar"), (0x12C1582, b"Small Vibration Scalar")):
        e.cstring(addr, text)
    require(e.read(0x12C159E, 1) == b"\0", "empty description points at a NUL byte")
    # the site's functor tag 0x15d10f0 and the accepted tag 0x15d57e0 share the same four slot bodies,
    # which store the class tag 0x1594318
    for base in (0x15D10F0, 0x15D57E0):
        for i, body in enumerate((0xC4A0, 0xC4C0, 0xC4E0, 0xC4F0)):
            require(e.reloc_target(base + 8 * i) == body, f"functor vtable {base:#x} slot {i}")
    require(e.rip(0xC4A4, 7, 3) == 0x1594318, "functor copy stores the class tag 0x1594318")
    require(e.rip(0xC4C0, 7, 3) == 0x1594318, "functor clone stores the class tag 0x1594318")
    e.bytes_at(0xC4F0, "c3", "functor destroy is a bare ret")
    # float class 0x15942a0 (+0x10 = 0x15942b0): slot 0 = dtor 0x20f0, 2 = register, 3 = 1, 6 = 3
    require(e.reloc_target(0x15942B0) == 0x20F0, "float class slot 0 = 0x20f0")
    require(e.reloc_target(0x15942B0 + 0x10) == 0x20D60, "float class slot 2 = 0x20d60")
    require(e.reloc_target(0x15942B0 + 0x18) == 0xBBA0, "float class slot 3 = 0xbba0 (returns 1)")
    require(e.reloc_target(0x15942B0 + 0x30) == 0xBBF0, "float class slot 6 = 0xbbf0 (returns 3)")
    e.bytes_at(0xBBA0, "b801000000c3", "float slot 3: mov eax,1; ret")
    e.bytes_at(0xBBF0, "b803000000c3", "float slot 6: mov eax,3; ret")
    # the 0x26ea0 default range / slot gate and the narrowed range {0.1f, 0.5f} it selects
    t = 0x26EA0
    e.bytes_at(t + 0x89, "c783980000000000803f" "c7839c00000000002041", "0x26ea0 default range {1.0f, 10.0f}")
    e.bytes_at(t + 0xA4, "4c89f7" "498b06" "ff5018" "83f801" "751f" "498b06" "4c89f7" "ff5030" "83f803" "7511",
               "0x26ea0 virtual slot 3 == 1 && slot 6 == 3 gate")
    e.bytes_at(0x26F60, "48b8cdcccc3d0000003f" "48898398000000", "0x26ea0 narrowed range qword 0x3f0000003dcccccd")
    # object placement is contiguous with the native layout
    require(0x32F4C38 + 0xD8 == 0x32F4D10 and 0x32F4D10 + 0xD8 == 0x32F4DE8 and 0x32F4DE8 + 0xA8 == 0x32F4E90
            and 0x32F4E90 + 0xA8 == 0x32F4F38, "object placement")
    require(0x32F4C38 + 0xD0 == 0x32F4D08 and 0x32F4D10 + 0xD0 == 0x32F4DE0, "value cells")


SPEC = Spec(
    name="slot317", index=317, base=BASE, size=SIZE, sha=SHA, pins=PINS, callees=CALLEES, elf_extra=elf_extra,
    native_files={"header": "native/core/ctor_slot_317_a2c390.hpp", "source": "native/core/ctor_slot_317_a2c390.cpp",
                  "test": "native/tests/test_ctor_slot_317.cpp", "cmake": "native/CMakeLists.txt"},
    header_constants={"kSlot317Rva": BASE, "kSlot317BodySize": SIZE, "kSlot317LargeRva": 0x32F4C38,
                      "kSlot317SmallRva": 0x32F4D10, "kSlot317GroupRva": 0x32F4DE8,
                      "kSlot317Toggle1Rva": 0x32F4E90, "kSlot317Toggle2Rva": 0x32F4F38,
                      "kSlot317ArgFnVtbl": 0x15D10F0, "kSlot317ArgFnEquivalent": 0x15D57E0,
                      "kSlot317DefaultBits": 0x3F000000, "kSlot317VarFlags": 1, "kSlot317ItemFlags": 0x30,
                      "kFloatClassSlot3Rva": 0xBBA0, "kFloatClassSlot6Rva": 0xBBF0,
                      "kFloatClassSlot3Result": 1, "kFloatClassSlot6Result": 3},
    header_tokens=['kSlot317LargeName[] = "vibration_large_scalar"', 'kSlot317SmallName[] = "vibration_small_scalar"',
                   'kSlot317Parent[] = "Platform Services"', 'kSlot317Caption[] = "Controller"',
                   'kSlot317Toggle1Text[] = "Large Vibration Scalar"',
                   'kSlot317Toggle2Text[] = "Small Vibration Scalar"', "struct Slot317Objects"],
    source_order=["init_float_var_static(o.large, float_spec(kSlot317LargeName), concmd)",
                  "init_float_var_static(o.small, float_spec(kSlot317SmallName), concmd)",
                  "init_menu_group_26c10(o.group, parent, caption, kSlot317ItemFlags)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.group)",
                  "init_debug_menu_item_26ea0(o.toggle1, label1, text1, float_var_menu_target(o.large)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle1)",
                  "init_debug_menu_item_26ea0(o.toggle2, label2, text2, float_var_menu_target(o.small)",
                  "record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle2)"],
    test_tokens=["kSlot317Rva == 0xa2c390", "section_build()", "section_failures()", "{0x272d0, &o.toggle2}"],
    cmake_tokens=["core/ctor_slot_317_a2c390.cpp", "add_executable(test_ctor_slot_317 tests/test_ctor_slot_317.cpp)",
                  "add_test(NAME ctor_slot_317 COMMAND test_ctor_slot_317)"],
    summary="vibration_large/small_scalar (unbounded float 0.5 via 0x6aa00) + Controller group + 2 narrowed-range toggles",
)

if __name__ == "__main__":
    raise SystemExit(main(SPEC))
