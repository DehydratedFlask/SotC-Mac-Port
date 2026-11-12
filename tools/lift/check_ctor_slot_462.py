#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 462 (RVA 0x3e2730, 2,580 B).

Batch 27. The body builds the point-of-interest / presentation debug page: a zero cell at 0x2ef7420,
two static views at 0x2ef7428 / 0x2ef7448, six commands (0x2ef7468 activatePOI, 0x2ef7508 clearPOI,
0x2ef7770 poiClearErrorMessages, 0x2ef7ab0 poiGroup, 0x2ef7b50 poi, plus the 0xd0-byte functor
command at 0x2ef76a0 built by 0x6af60), one bool variable at 0x2ef75a8, a 0x30-byte string holder
at 0x2ef7670, and four menu items at 0x2ef7810 / 0x2ef78b8 / 0x2ef7960 / 0x2ef7a08.

Fifteen __cxa_atexit records in body order: 0x3e01e0, 0xf7b0, 0xf7b0, 0x4b0, 0x4b0, 0x20b0,
0x8ef00, 0x54ba0, 0x4b0, 0x272d0 x2, 0x272d0 x2, 0x4b0, 0x4b0. Seven 0x20d60 registrations: six
commands and the bool variable. The functor command is modelled by concmd_functor_6af60.

Offline only: reads the pinned private ELF, never executes an original RVA.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

BASE = 0x3E2730
SIZE = 2580
ATEXIT = 0xF8F390

# Every direct e8 in the body, in order (site -> target). The last is __stack_chk_fail.
CALLS = [
    (0x3E2779, ATEXIT), (0x3E27B7, ATEXIT), (0x3E27EE, ATEXIT), (0x3E2807, 0xF2F0),
    (0x3E286C, 0x20AF0), (0x3E28C9, 0xF590), (0x3E28D6, 0xF590), (0x3E28EC, ATEXIT),
    (0x3E2905, 0xF2F0), (0x3E295D, 0x20AF0), (0x3E29B5, 0xF590), (0x3E29C2, 0xF590),
    (0x3E29DB, ATEXIT), (0x3E2A57, 0x20AF0), (0x3E2AB9, 0xF590), (0x3E2AD6, 0x20D60),
    (0x3E2ADE, 0xF590), (0x3E2AF3, ATEXIT), (0x3E2B72, 0xF590), (0x3E2B84, ATEXIT),
    (0x3E2C45, 0x6AF60), (0x3E2C51, 0xF590), (0x3E2C67, 0xF590), (0x3E2C7D, ATEXIT),
    (0x3E2C96, 0xF2F0), (0x3E2CEE, 0x20AF0), (0x3E2D48, 0xF590), (0x3E2D55, 0xF590),
    (0x3E2D6E, ATEXIT), (0x3E2DC7, 0x26EA0), (0x3E2DCF, 0xF590), (0x3E2DE1, ATEXIT),
    (0x3E2E56, 0x26F90), (0x3E2E5E, 0xF590), (0x3E2E66, 0xF590), (0x3E2E7B, ATEXIT),
    (0x3E2ECD, 0x26EA0), (0x3E2ED5, 0xF590), (0x3E2EE3, ATEXIT), (0x3E2F2D, 0x26C10),
    (0x3E2F35, 0xF590), (0x3E2F43, ATEXIT), (0x3E2F5C, 0xF2F0), (0x3E2FB9, 0x20AF0),
    (0x3E3011, 0xF590), (0x3E301E, 0xF590), (0x3E3034, ATEXIT), (0x3E304D, 0xF2F0),
    (0x3E30A5, 0x20AF0), (0x3E30F3, 0xF590), (0x3E3100, 0xF590), (0x3E3116, ATEXIT),
    (0x3E313E, 0xF8F3A0),
]

OBJECT_LEAS = [
    (0x3E2756, 0x2EF7420), (0x3E278C, 0x2EF7428), (0x3E27C3, 0x2EF7448),
    (0x3E280C, 0x2EF7468), (0x3E290A, 0x2EF7508), (0x3E29EE, 0x2EF75A8),
    (0x3E2B12, 0x2EF7670), (0x3E2B9B, 0x2EF76A0), (0x3E2C9B, 0x2EF7770),
    (0x3E2D73, 0x2EF7810), (0x3E2DE6, 0x2EF78B8), (0x3E2E80, 0x2EF7960),
    (0x3E2EE8, 0x2EF7A08), (0x3E2F61, 0x2EF7AB0), (0x3E3052, 0x2EF7B50),
]

PINS = [("imm", BASE, "55"), ("imm", 0x3E313E, "e8")]
for site, target in CALLS:
    PINS.append(("call", site, target))
for site, target in OBJECT_LEAS:
    PINS.append(("rip", site, 7, 3, target))
PINS += [
    ("rip", 0x3E29E0, 7, 3, 0x1594208),   # bool pre-vptr
    ("rip", 0x3E2A5C, 7, 3, 0x15940A0),   # bool mid-vptr
    ("rip", 0x3E2A71, 7, 3, 0x15B18C8),   # bool functor tag
    ("rip", 0x3E2AF8, 7, 3, 0x15976A0),   # string-holder vtable
    ("rip", 0x3E2871, 7, 3, 0x1594DC0),   # command vtable
    ("rip", 0x3E2D1F, 7, 3, 0x1594DC0),
    ("rip", 0x3E2B89, 7, 3, 0x2EA8EE8),   # functor spec (.bss)
    ("rip", 0x3E2BE9, 7, 3, 0x15B18E8),   # functor view vtable
    ("rip", 0x3E2DED, 7, 3, 0x2DFCBD0),   # opaque 0x26f90 target
]

SPEC = {
    "label": "462",
    "slot": 462,
    "base": BASE,
    "size": SIZE,
    "body_sha256": "a82b6a8828e6cd3cb516c8f21dc532351c95bf3d2034c22177cd06d37c90ffcc",
    "pad": "90" * 12,
    "callees": ["concmd_base_ctor", "concmd_register", "release_f590"],
    "bool_tags": [0x15B18C8],
    "float_tags": [],
    "strings": [
        (0xFE92E0, b"Presentation"),
        (0xFE92ED, b"Clear Active Point of Interest"),
        (0xFE930C, b"activatePOI"),
        (0xFE9318, b"Sets the poi as active. +activatePOI <group> <name>"),
        (0xFE934C, b"clearPOI"),
        (0xFE9355, b"Sets no poi as active. +clearPOI"),
        (0xFE9376, b"showPoiDescription"),
        (0xFE9389, b"renderErrorMsgs"),
        (0xFE9399, b"presentationShowRenderTarget"),
        (0xFE93B7, b"poiClearErrorMessages"),
        (0xFE93CD, b"Clear Error Messages"),
        (0xFE93E2, b"Show Description"),
        (0xFE93F3, b"Show Render Target"),
        (0xFE9406, b"Shadow Boss Jump"),
        (0xFE94AD, b"poiGroup"),
        (0xFE94B6, b"Add a point of interest group. +poiGroup <name> <description>"),
        (0xFE94F4, b"poi"),
        (0xFE94F8, b"Add a point of interest the debug camera can use to jump to. +poi <group> "
                    b"<name> <posx> <posy> <posz> <opt:size> <opt:description>"),
    ],
    "lengths": [],
    "pins": PINS,
    "native": {
        "header": "native/core/ctor_slot_462_3e2730.hpp",
        "source": "native/core/ctor_slot_462_3e2730.cpp",
        "test": "native/tests/test_ctor_slot_462.cpp",
        "cmake": "native/CMakeLists.txt",
        "consts": {
            "kSlot462Rva": BASE,
            "kSlot462Size": SIZE,
            "kSlot462TableWord": 0x2DD6270,
            "kSlot462ZeroCell": 0x2EF7420,
            "kSlot462Presentation": 0x2EF7428,
            "kSlot462ClearActive": 0x2EF7448,
            "kSlot462Activate": 0x2EF7468,
            "kSlot462Clear": 0x2EF7508,
            "kSlot462ShowDescription": 0x2EF75A8,
            "kSlot462RenderErrors": 0x2EF7670,
            "kSlot462ShowRenderTarget": 0x2EF76A0,
            "kSlot462ClearErrors": 0x2EF7770,
            "kSlot462ItemShowDescription": 0x2EF7810,
            "kSlot462ItemClearErrors": 0x2EF78B8,
            "kSlot462ItemShowTarget": 0x2EF7960,
            "kSlot462ItemShadowJump": 0x2EF7A08,
            "kSlot462PoiGroup": 0x2EF7AB0,
            "kSlot462Poi": 0x2EF7B50,
            "kSlot462CommandVptr": 0x1594DD0,
            "kSlot462BoolMidVptr": 0x15940B0,
            "kSlot462BoolFinalVptr": 0x1594218,
            "kSlot462StringVptr": 0x15976B0,
            "kSlot462BoolFnTag": 0x15B18C8,
            "kSlot462CommandTail8": 0x1990,
            "kSlot462CommandTail10": 0x2512A0,
            "kSlot462ZeroCellDtor": 0x3E01E0,
            "kSlot462ViewDtor": 0xF7B0,
            "kSlot462CommandDtor": 0x4B0,
            "kSlot462BoolDtor": 0x20B0,
            "kSlot462StringDtor": 0x8EF00,
            "kSlot462FunctorDtor": 0x54BA0,
            "kSlot462MenuItemDtor": 0x272D0,
            "kSlot462ActivateCallback": 0x3E0600,
            "kSlot462ClearCallback": 0x3E0810,
            "kSlot462ClearErrorsCallback": 0x3E0840,
            "kSlot462PoiGroupCallback": 0x3E1CB0,
            "kSlot462PoiCallback": 0x3E1E10,
            "kSlot462AtexitRva": ATEXIT,
            "kSlot462CanaryCell": 0x15FE518,
            "kSlot462StackChkFailRva": 0xF8F3A0,
        },
        "header_tokens": [
            "0x3e2730", "0x2ef7420", "0x2ef75a8", "0x2ef76a0", "0x2ef7810",
            "0x1594dd0", "0x1594218", "0x15976b0", "0x6af60", "0x5c6b0", "0x5c830",
            "INFERENCE",
        ],
        "source_order": [
            "family_view", "init_concmd_base_20af0", "register_concmd_20d60",
            "init_debug_menu_item_26ea0", "register_menu_item_268b0",
            "init_concmd_functor_6af60", "init_menu_command_26f90", "init_menu_group_26c10",
        ],
        "test_tokens": [
            "kSlot462", "functor_transfers", "atexit_log.at", "item_show_description",
        ],
        "cmake_tokens": [
            "concmd_functor_6af60.cpp", "ctor_slot_462_3e2730.cpp",
            "test_concmd_functor_6af60.cpp", "test_ctor_slot_462.cpp",
        ],
    },
}

SUMMARY = ("a zero cell, two static views, six commands (including the 0xd0-byte 0x6af60 functor "
           "command), one bool variable, one string holder, four menu items and fifteen __cxa_atexit "
           "records; seven 0x20d60 registrations")


def check_semantics(elf, header, verbose=True):
    require = lib.require
    body = elf.raw(BASE, SIZE)
    seen = []
    for i in range(len(body) - 5):
        if body[i] == 0xE8:
            site = BASE + i
            # 0x3e2b0f is the second opcode byte of `c5 e8 57 d2` (vxorps xmm2,xmm2,xmm2) at
            # 0x3e2b0e, not a call. It is the only non-call 0xe8 byte in the body.
            if site == 0x3E2B0F:
                require(body[i - 1:i + 3] == b"\xc5\xe8\x57\xd2", "0x3e2b0f vxorps bytes")
                continue
            target = site + 5 + int.from_bytes(body[i + 1:i + 5], "little", signed=True)
            seen.append((site, target))
    require(seen == CALLS, f"call sequence: {[(hex(a), hex(b)) for a, b in seen]}")
    require(sum(1 for _, target in seen if target == ATEXIT) == 15, "fifteen __cxa_atexit records")
    require(sum(1 for _, target in seen if target == 0x20D60) == 1, "one direct 0x20d60 (the bool)")
    require(sum(1 for _, target in seen if target == 0x20AF0) == 6, "six base ctors")
    require(sum(1 for _, target in seen if target == 0xF2F0) == 5, "five C-string names")
    require(sum(1 for _, target in seen if target == 0x26EA0) == 2, "two toggle ctors")
    require(sum(1 for _, target in seen if target == 0x26F90) == 1, "one command item")
    require(sum(1 for _, target in seen if target == 0x26C10) == 1, "one group item")
    require(sum(1 for _, target in seen if target == 0x6AF60) == 1, "one functor-command helper")
    require(elf.u64(0x2DD70E0 - 8 * 462) == BASE, "constructor table word 462")
    for rva, text in SPEC["strings"]:
        require(elf.read(rva, len(text)) == text, f"rodata at {rva:#x}")
    require(elf.read(0xFE93B6, 1) == b"\x00", "empty description string")
    if verbose:
        print("  oracle: 53 call sites, 15 atexit rows, 7 registrations, 6 base ctors, 2 toggles, "
              "1 command item, 1 group; strings re-derived from rodata")


def main() -> int:
    argv = sys.argv[1:]
    elf = lib.Elf()
    header = (lib.ROOT / SPEC["native"]["header"]).read_text()

    if "--tamper-native" in argv:
        mutated = header.replace('"Presentation"', '"Presentatio5"')
        if mutated == header:
            raise SystemExit("FAIL --tamper-native could not edit the header")
        lib.check_native(SPEC, {"header": mutated,
                                "source": (lib.ROOT / SPEC["native"]["source"]).read_text(),
                                "test": (lib.ROOT / SPEC["native"]["test"]).read_text(),
                                "cmake": (lib.ROOT / SPEC["native"]["cmake"]).read_text()})
        print("tamper-native NOT detected", file=sys.stderr)
        return 0

    if "--tamper" not in argv:
        check_semantics(elf, header)
        print("  ok: 6 commands + 1 bool, 4 menu items, 15 atexit rows")
    return lib.main(SPEC, SUMMARY)


if __name__ == "__main__":
    sys.exit(main())
