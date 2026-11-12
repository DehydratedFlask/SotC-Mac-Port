#!/usr/bin/env python3
"""ELF-tie checker for the seven holder dtor twins 0x8ef00 / 0x2fced0 / 0x2b5050 / 0x34e820 / 0x5b5470 / 0x5b5490 / 0x2c84f0 (CPU-01-SLOT408-PREREQS).

Pins each 23 B body per instruction, ties the vptr class and the 0xf590 tail jump, and re-derives the startup-slot consumers.

Usage: python3 -I tools/lift/check_helper_8ef00.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_f as pins_f  # noqa: E402
from helper_f_util import header_hex_list, slot_ties  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.pins.PINS.update(pins_f.PINS)

SLOTS = [("slot 408 (0x5c7f50)", 0x5c7f50, 4750, "73c66ded4a5facf4c6ab4df025f86bc043455dad9e4a3dcd9f25ca50982201eb"), ("slot 473 (0x355a00)", 0x355a00, 6487, "2ec7a932dd8bed1a4b7cfc24091e926e6068bcd295a6124e9aaa3a86dd11fcf8")]
SITES = [
    ("slot 408 dtor 0x5b5470", 0x5C7F50, 4750, 0x5C8371, 0x5B5470), ("slot 408 dtor 0x5b5490", 0x5C7F50, 4750, 0x5C83F3, 0x5B5490),
    ("slot 408 dtor 0x2c84f0", 0x5C7F50, 4750, 0x5C82EF, 0x2C84F0), ("slot 473 dtor 0x34e820", 0x355A00, 6487, 0x355A9A, 0x34E820),
]
TWINS = [0x8ef00, 0x2fced0, 0x2b5050, 0x34e820, 0x5b5470, 0x5b5490, 0x2c84f0]


def extra(e, fams) -> None:
    slot_ties(e, SLOTS, SITES)
    listed = header_hex_list("native/core/helper_8ef00_holder_dtor.hpp", "kHolderDtorTwins")
    require(listed == TWINS, f"native twin table {[hex(x) for x in listed]} equals the ELF-pinned twins")


SPEC = {
    "name": "check_helper_8ef00",
    "summary": ("seven 23 B holder dtor twins pinned per instruction (vptr class 0x1597648 + 0x10, tail release 0xf590); the three "
                "slot 408 dtors 0x5b5470 / 0x5b5490 / 0x2c84f0 and slot 473's 0x34e820 are re-derived as lea sites in their slot bodies"),
    "families": [],
    "bodies": [
        {"name": "0x8ef00 holder dtor", "rva": 0x8ef00, "size": 23, "fde": 23, "pins": "holder_dtor_8ef00",
         "sha": "32799b7bccf74b604c28f179e15a8bfb5b928f37bfacc3efa336f33d022e6c86"},
        {"name": "0x2fced0 holder dtor", "rva": 0x2fced0, "size": 23, "fde": 23, "pins": "holder_dtor_2fced0",
         "sha": "1f337bdbfd5da11f79e5c63a80992685b281d32a8b91df74cbf786896f2cbc21"},
        {"name": "0x2b5050 holder dtor", "rva": 0x2b5050, "size": 23, "fde": 23, "pins": "holder_dtor_2b5050",
         "sha": "e2e804fcfa43736ffe5be8e262e95449375c8ddae94f1ea867be29efeae6b1de"},
        {"name": "0x34e820 holder dtor", "rva": 0x34e820, "size": 23, "fde": 23, "pins": "holder_dtor_34e820",
         "sha": "cf2700e83e0fadafd1acb376b1f5bcf92a99b21fc867de6f233f078beeaff504"},
        {"name": "0x5b5470 holder dtor", "rva": 0x5b5470, "size": 23, "fde": 23, "pins": "holder_dtor_5b5470",
         "sha": "9417c8c3352b230348a513c1ebfe1048e1da4421c1ec74c501cb34c62ed61f38"},
        {"name": "0x5b5490 holder dtor", "rva": 0x5b5490, "size": 23, "fde": 23, "pins": "holder_dtor_5b5490",
         "sha": "e7ce243065bee3cd76451d27f0822387dd6232c2b1f04f18c20188eb491e019a"},
        {"name": "0x2c84f0 holder dtor", "rva": 0x2c84f0, "size": 23, "fde": 23, "pins": "holder_dtor_2c84f0",
         "sha": "2050283851e71b96a7d48d2a737f4dcdd1e9ead367c9d8af0740269be4ad14d6"},
    ],
    "rips": [
        ("0x8ef00 vptr lea", 0x8ef00, 7, 3, 0x1597648),
        ("0x2fced0 vptr lea", 0x2fced0, 7, 3, 0x1597648),
        ("0x2b5050 vptr lea", 0x2b5050, 7, 3, 0x1597648),
        ("0x34e820 vptr lea", 0x34e820, 7, 3, 0x1597648),
        ("0x5b5470 vptr lea", 0x5b5470, 7, 3, 0x1597648),
        ("0x5b5490 vptr lea", 0x5b5490, 7, 3, 0x1597648),
        ("0x2c84f0 vptr lea", 0x2c84f0, 7, 3, 0x1597648),
    ],
    "calls": [],
    "jmps": [
        ("0x8ef00 tail jmp release", 0x8ef12, 0xF590),
        ("0x2fced0 tail jmp release", 0x2fcee2, 0xF590),
        ("0x2b5050 tail jmp release", 0x2b5062, 0xF590),
        ("0x34e820 tail jmp release", 0x34e832, 0xF590),
        ("0x5b5470 tail jmp release", 0x5b5482, 0xF590),
        ("0x5b5490 tail jmp release", 0x5b54a2, 0xF590),
        ("0x2c84f0 tail jmp release", 0x2c8502, 0xF590),
    ],
    "relocs": [],
    "bytes": [
        ("0x8ef00 `add rax,0x10 ; mov [rdi],rax ; add rdi,8`", 0x8ef07, "4883c010488907"),
        ("0x2fced0 `add rax,0x10 ; mov [rdi],rax ; add rdi,8`", 0x2fced7, "4883c010488907"),
        ("0x2b5050 `add rax,0x10 ; mov [rdi],rax ; add rdi,8`", 0x2b5057, "4883c010488907"),
        ("0x34e820 `add rax,0x10 ; mov [rdi],rax ; add rdi,8`", 0x34e827, "4883c010488907"),
        ("0x5b5470 `add rax,0x10 ; mov [rdi],rax ; add rdi,8`", 0x5b5477, "4883c010488907"),
        ("0x5b5490 `add rax,0x10 ; mov [rdi],rax ; add rdi,8`", 0x5b5497, "4883c010488907"),
        ("0x2c84f0 `add rax,0x10 ; mov [rdi],rax ; add rdi,8`", 0x2c84f7, "4883c010488907"),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_8ef00_holder_dtor.hpp", "helper_c": "native/core/helper_8ef00_holder_dtor.cpp",
        "test": "native/tests/test_helper_8ef00.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kHolderDtorSize": 23, "kHolderDtorClass": 0x1597648, "kHolderDtorVptr": 0x1597658,
                     "kHolderDtorHolderOffset": 8, "kHolderDtorObjectSize": 0x28},
    },
    "tokens": {
        "helper_h": ["std::array<std::uint64_t, 7> kHolderDtorTwins", "SharedHolderF590 holder;"],
        "helper_c": ["object.vptr = kHolderDtorVptr;", "shared_release_f590(object.holder, hooks)"],
        "test": ["CHECK(obj.vptr == kHolderDtorVptr", "CHECK(!r.ok && same_bytes(before, other));"],
        "cmake": ["core/helper_8ef00_holder_dtor.cpp", "add_test(NAME helper_8ef00 COMMAND test_helper_8ef00)"],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
