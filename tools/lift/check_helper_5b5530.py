#!/usr/bin/env python3
"""ELF-tie checker for the slot 408 vector dtors 0x5b5530 / 0x5b55c0 / 0x5c7c00 (CPU-01-SLOT408-PREREQS).

Pins the three bodies per instruction (SSO-string vector, inline-storage guard, 0x30 B buffer-element vector), ties each free (0x170b0 / 0x170c0) and re-derives slot 408's three atexit references (0x5c840c, 0x5c8433, 0x5c9193).

Usage: python3 -I tools/lift/check_helper_5b5530.py [--no-mutate] [--tamper elf|native]
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
    ("slot 408 (0x5c7f50)", 0x5c7f50, 4750, "73c66ded4a5facf4c6ab4df025f86bc043455dad9e4a3dcd9f25ca50982201eb"),
]
SITES = [
    ("slot 408 dtor 0x5b5530", 0x5c7f50, 4750, 0x5c840c, 0x5b5530),
    ("slot 408 dtor 0x5b55c0", 0x5c7f50, 4750, 0x5c8433, 0x5b55c0),
    ("slot 408 dtor 0x5c7c00", 0x5c7f50, 4750, 0x5c9193, 0x5c7c00),
]


def extra(e, fams) -> None:
    slot_ties(e, SLOTS, SITES)


SPEC = {
    "name": "check_helper_5b5530",
    "summary": ('three slot 408 vector dtors (0x5b5530 SSO-string vector, 0x5b55c0 inline-storage guard, 0x5c7c00 0x30 B buffer vector) pinned per instruction; frees tied; slot 408 atexit references 0x5c840c / 0x5c8433 / 0x5c9193 re-derived'),
    "families": [],
    "bodies": [
        {"name": "0x5b5530 SSO vector dtor", "rva": 0x5b5530, "size": 142, "fde": 142, "pins": "sso_vector_dtor_5b5530",
         "sha": "904f71d54f4a00c8077f5937fa716e9e962cd91e2b45a79e86b75788f43e7e24"},
        {"name": "0x5b55c0 inline guard dtor", "rva": 0x5b55c0, "size": 23, "fde": 23, "pins": "inline_guard_dtor_5b55c0",
         "sha": "aec5e8c779e1370c4bf7b09b0d7c48e6cbfd13dc4758effc9cc80280d6969a3a"},
        {"name": "0x5c7c00 vec30 dtor", "rva": 0x5c7c00, "size": 127, "fde": 127, "pins": "vec30_dtor_5c7c00",
         "sha": "e775b54a5c469842b5e0d8aeaa5f42ecd015c0c7e0d71d667682d037a4f2f386"},
    ],
    "rips": [
    ],
    "calls": [
        ('0x5b5530 element heap free', 0x5b557b, 0x170b0),
        ('0x5b5530 vector buffer free', 0x5b559d, 0x170b0),
        ('0x5c7c00 element buffer free', 0x5c7c39, 0x170b0),
        ('0x5c7c00 vector buffer free', 0x5c7c5e, 0x170b0),
    ],
    "jmps": [
        ('0x5b55c0 tail free type 2', 0x5b55d1, 0x170c0),
    ],
    "relocs": [],
    "bytes": [
        ('0x5b5530 `cmp qword [rbx+0x20],0x10`', 0x5b5570, '48837b2010'),
        ('0x5b5530 `mov eax,0xf`', 0x5b5556, 'b80f000000'),
        ('0x5b5530 `vpslldq xmm0,8`', 0x5b5560, 'c5f973f808'),
        ('0x5b5530 `mov byte [rbx+8],0`', 0x5b558a, 'c6430800'),
        ('0x5b5530 `add rbx,0x28`', 0x5b558e, '4883c328'),
        ('0x5b5530 `mov qword [r14+0x10],0`', 0x5b55ab, '49c7461000000000'),
        ('0x5b55c0 `cmp rdi,[rax+0x20]`', 0x5b55cb, '48397820'),
        ('0x5c7c00 `add rbx,0x30`', 0x5c7c4f, '4883c330'),
        ('0x5c7c00 `mov qword [rbx+0x28],0`', 0x5c7c47, '48c7432800000000'),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_5b5530_vector_dtors.hpp", "helper_c": "native/core/helper_5b5530_vector_dtors.cpp",
        "test": "native/tests/test_helper_5b5530.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {'kSsoVectorDtor5b5530Rva': 5985584, 'kSsoVectorDtor5b5530Size': 142, 'kInlineGuardDtor5b55c0Rva': 5985728, 'kInlineGuardDtor5b55c0Size': 23, 'kVec30Dtor5c7c00Rva': 6061056, 'kVec30Dtor5c7c00Size': 127, 'kSsoElementSize': 40, 'kSsoElementBufferOffset': 8, 'kSsoElementSizeFieldOffset': 24, 'kSsoElementCapacityOffset': 32, 'kSsoInlineCapacity': 15, 'kVec30ElementSize': 48, 'kVec30ElementBufferOffset': 24, 'kVectorHeaderOffset': 8},
    },
    "tokens": {
        "helper_c": ['if (load_u64(element, kSsoElementCapacityOffset) >= 0x10)', 'store_u64(element, kSsoElementCapacityOffset, kSsoInlineCapacity);', 'hooks.free_type1(begin);', 'hooks.free_type2(pointer);', 'pointer == load_ptr(object, 0x20)'],
        "test": ['CHECK(get_u64(obj, 8) == 0 && get_u64(obj, 0x10) == 0 && get_u64(obj, 0x18) == 0);', 'put(cell, 0, elems); put(cell, 0x20, elems);'],
        "cmake": ['core/helper_5b5530_vector_dtors.cpp', 'add_test(NAME helper_5b5530 COMMAND test_helper_5b5530)'],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
