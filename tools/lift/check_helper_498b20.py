#!/usr/bin/env python3
"""ELF-tie checker for the five ranged console-variable dtors 0x498b20 / 0x498b60 / 0x23d20 / 0x13a090 / 0x140010 (CPU-01-SLOT408-PREREQS).

Pins the five 50/54 B bodies per instruction (vptr, functor destroy through [[obj+0xc0]+0x18], tail jump to the base dtor 0x20b50) and re-derives their startup-slot consumers (slots 451, 566 and 534).

Usage: python3 -I tools/lift/check_helper_498b20.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_f as pins_f  # noqa: E402
from helper_f_util import slot_ties  # noqa: E402

lib.pins.PINS.update(pins_f.PINS)

SLOTS = [
    ("slot 451 (0x49d230)", 0x49d230, 5126, "f6655116c4174f7d116b24f8f54f4beb69727ef64b920df63eaf59e45c504b59"),
    ("slot 566 (0x25de0)", 0x25de0, 1194, "9a520d7e1eb8f77bb73d50e0b61f60c340e28ace39adbdd70e3f444b7150d2b0"),
    ("slot 534 (0x1418d0)", 0x1418d0, 6207, "4eedf09f4d802ac8cbbd255206623722c2bea3f484ed3081cfc622c5581b968c"),
]
SITES = [
    ("slot 451 dtor 0x498b20", 0x49d230, 5126, 0x49dbf9, 0x498b20),
    ("slot 451 dtor 0x498b60", 0x49d230, 5126, 0x49dfc7, 0x498b60),
    ("slot 566 dtor 0x23d20", 0x25de0, 1194, 0x261c3, 0x23d20),
    ("slot 534 dtor 0x13a090 #0", 0x1418d0, 6207, 0x141c1f, 0x13a090),
    ("slot 534 dtor 0x13a090 #1", 0x1418d0, 6207, 0x141d21, 0x13a090),
    ("slot 534 dtor 0x13a090 #2", 0x1418d0, 6207, 0x142458, 0x13a090),
    ("slot 534 dtor 0x140010", 0x1418d0, 6207, 0x142ed4, 0x140010),
]


def extra(e, fams) -> None:
    slot_ties(e, SLOTS, SITES)


SPEC = {
    "name": "check_helper_498b20",
    "summary": ('five ranged console-variable dtors (vptr store, functor destroy [[obj+0xc0]+0x18](obj+0xa0), tail jump to 0x20b50) pinned per instruction; consumers in slots 451 / 534 / 566 re-derived'),
    "families": [],
    "bodies": [
        {"name": "0x498b20 ranged var dtor", "rva": 0x498b20, "size": 54, "fde": 54, "pins": "ranged_var_dtor_498b20",
         "sha": "af57e4022985c0330813221d1ec49516a755ecef695e4c81dfed85d708130a57"},
        {"name": "0x498b60 ranged var dtor", "rva": 0x498b60, "size": 54, "fde": 54, "pins": "ranged_var_dtor_498b60",
         "sha": "c6c26e8c0aec70286ab05f9da73302cac2e3bcc91bc575ad2f22977cbd366b1d"},
        {"name": "0x23d20 ranged var dtor", "rva": 0x23d20, "size": 54, "fde": 54, "pins": "ranged_var_dtor_23d20",
         "sha": "e7deb5eca625013f5e7218facf26e32353c7d579831b16e0d645341dce169c5c"},
        {"name": "0x13a090 ranged var dtor", "rva": 0x13a090, "size": 54, "fde": 54, "pins": "ranged_var_dtor_13a090",
         "sha": "66f421ddb516bd3193858071f60f9e65ed6a5af876632bb27abe88b1ba6bfe60"},
        {"name": "0x140010 ranged var dtor", "rva": 0x140010, "size": 50, "fde": 50, "pins": "ranged_var_dtor_140010",
         "sha": "6d94629ffb98aa86495c237e2b2797c43881922db7b6422718167ec41b7d7050"},
    ],
    "rips": [
        ('0x498b20 vptr class', 0x498b26, 7, 3, 0x15bfb20),
        ('0x498b60 vptr class', 0x498b66, 7, 3, 0x15bfb98),
        ('0x23d20 vptr class', 0x23d26, 7, 3, 0x1594fd0),
        ('0x13a090 vptr class', 0x13a096, 7, 3, 0x1598240),
        ('0x140010 vptr literal', 0x140019, 7, 3, 0x15982c8),
    ],
    "calls": [
    ],
    "jmps": [
        ('0x498b20 base dtor jmp', 0x498b51, 0x20b50),
        ('0x498b60 base dtor jmp', 0x498b91, 0x20b50),
        ('0x23d20 base dtor jmp', 0x23d51, 0x20b50),
        ('0x13a090 base dtor jmp', 0x13a0c1, 0x20b50),
        ('0x140010 base dtor jmp', 0x14003d, 0x20b50),
    ],
    "relocs": [],
    "bytes": [
        ('0x498b20 `mov rax,[rbx+0xc0] ; call [rax+0x18]`', 0x498b3e, '488b83c0000000ff5018'),
        ('0x498b60 same', 0x498b7e, '488b83c0000000ff5018'),
        ('0x23d20 same', 0x23d3e, '488b83c0000000ff5018'),
        ('0x13a090 same', 0x13a0ae, '488b83c0000000ff5018'),
        ('0x140010 `lea rdi,[rbx+0xa0] ; mov rax,[rbx+0xc0] ; call [rax+0x18]`', 0x140023, '488dbba0000000488b83c0000000ff5018'),
        ('0x498b20 `lea rdi,[rbx+0xa0]`', 0x498b30, '488dbba0000000'),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_498b20_ranged_var_dtors.hpp", "helper_c": "native/core/helper_498b20_ranged_var_dtors.cpp",
        "test": "native/tests/test_helper_498b20.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {'kRangedVarDtorFnOffset': 160, 'kRangedVarDtorFnVptrOffset': 192, 'kRangedVarDtorDestroySlotOffset': 24, 'kRangedVarBaseDtorRva': 133968, 'kRangedVarObjectSize': 200},
    },
    "tokens": {
        "helper_h": ['{0x498b20, 54, 0x15bfb30}', '{0x498b60, 54, 0x15bfba8}', '{0x23d20, 54, 0x1594fe0}', '{0x13a090, 54, 0x1598250}', '{0x140010, 50, 0x15982c8}'],
        "helper_c": ['hooks.functor_destroy(fn_vtbl, bytes + kRangedVarDtorFnOffset);', 'hooks.base_dtor(object);', 'std::memcpy(bytes, &row->vptr, sizeof row->vptr);'],
        "test": ['CHECK(g_log == "fn;base;");', 'find_ranged_var_dtor(0x140010)->vptr == 0x15982c8'],
        "cmake": ['core/helper_498b20_ranged_var_dtors.cpp', 'add_test(NAME helper_498b20 COMMAND test_helper_498b20)'],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
