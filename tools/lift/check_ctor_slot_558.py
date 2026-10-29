#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 558 (RVA 0x428a0, 230 bytes), lane M6.

Five zeroed qword cells (atexit 0x418c0), one inline small vector (begin = end = F + 0x20, end-of-storage F + 0x160,
atexit 0x41900) and one zeroed owning qword (tail __cxa_atexit 0x41910). No console objects, no strings.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every instruction operand of the
body (rip-relative targets, call/jmp targets, immediates, indirect calls; generated once from the
disassembly, re-verified here from bytes), the referenced strings, the functor vtables and the hash of
every shared callee / destructor. A mutation self-test flips the first/last byte of every pin and
perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lane_m6_lib as lib  # noqa: E402

SPEC = {
    "slot": 558,
    "base": 0x428a0,
    "size": 230,
    "body_sha256": "1483a64ae2dc8bdf1a9a92ceb2cca1179aede0717dbd93425c7c82fffbb9ec56",
    "pad": "90909090909090909090",
    "callees": [],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
    ],
    "lengths": [
    ],
    "pins": [
        ("imm", 0x428a1, "4889e5"),
        ("rip", 0x428a7, 7, 3, 0x418c0),
        ("rip", 0x428ae, 7, 3, 0x1604218),
        ("rip", 0x428b5, 7, 3, 0x2ea86a0),
        ("imm", 0x428bc, "4889df"),
        ("imm", 0x428bf, "4c89f2"),
        ("imm", 0x428c2, "48c70600000000"),
        ("call", 0x428c9, 0xf8f390),
        ("rip", 0x428ce, 7, 3, 0x2ea86a8),
        ("imm", 0x428d5, "4889df"),
        ("imm", 0x428d8, "4c89f2"),
        ("imm", 0x428db, "48c70600000000"),
        ("call", 0x428e2, 0xf8f390),
        ("rip", 0x428e7, 7, 3, 0x2ea86b0),
        ("imm", 0x428ee, "4889df"),
        ("imm", 0x428f1, "4c89f2"),
        ("imm", 0x428f4, "48c70600000000"),
        ("call", 0x428fb, 0xf8f390),
        ("rip", 0x42900, 7, 3, 0x2ea86b8),
        ("imm", 0x42907, "4889df"),
        ("imm", 0x4290a, "4c89f2"),
        ("imm", 0x4290d, "48c70600000000"),
        ("call", 0x42914, 0xf8f390),
        ("rip", 0x42919, 7, 3, 0x2ea86c0),
        ("imm", 0x42920, "4889df"),
        ("imm", 0x42923, "4c89f2"),
        ("imm", 0x42926, "48c70600000000"),
        ("call", 0x4292d, 0xf8f390),
        ("rip", 0x42932, 7, 3, 0x2ea86f8),
        ("rip", 0x42939, 7, 3, 0x41900),
        ("imm", 0x42940, "4c89f2"),
        ("imm", 0x42943, "488d4620"),
        ("imm", 0x42947, "c4e1f96ec0"),
        ("imm", 0x4294c, "488d8660010000"),
        ("imm", 0x42953, "c5f970c044"),
        ("imm", 0x42958, "c5fa7f06"),
        ("imm", 0x4295c, "48894610"),
        ("call", 0x42960, 0xf8f390),
        ("rip", 0x42965, 7, 3, 0x2ea8858),
        ("rip", 0x4296c, 7, 3, 0x41910),
        ("imm", 0x42973, "4c89f2"),
        ("imm", 0x42976, "48c70600000000"),
        ("jmp", 0x42981, 0xf8f390),
    ],
    "m6_callees": {
        "cell_dtor_418c0": (0x418c0, 0x34, "7ce1915da7173e36dcd5b82ddc84279095335d3b5144dd0188357cae741f443f"),
        "vector_dtor_41900": (0x41900, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
        "owner_dtor_41910": (0x41910, 0x25, "527056b5d040ccceb3ac1a2fe69496b640c6e89dfadb0d5383e651af6d14fae3"),
    },
    "native": {
        'header': 'native/core/ctor_slot_558_428a0.hpp',
        'source': 'native/core/ctor_slot_558_428a0.cpp',
        'test': 'native/tests/test_ctor_slot_558.cpp',
        'cmake': 'native/lanes/m6.cmake',
        'consts': {'kSlot558Rva': 272544, 'kSlot558Size': 230, 'kSlot558CellDtorRva': 268480, 'kSlot558VectorDtorRva': 268544, 'kSlot558OwnerDtorRva': 268560, 'kSlot558CellCount': 5, 'kSlot558VectorStorageBytes': 320},
        'header_tokens': ['sizeof(Slot558InlineVector) == 0x160 && offsetof(Slot558InlineVector, storage) == 0x20'],
        'source_order': ['cell = 0;', 'registry.atexit_log.record(kSlot558CellDtorRva, &cell)', 'v.begin = v.storage;', 'v.end = v.storage;', 'v.storage_end = v.storage + kSlot558VectorStorageBytes;', 'registry.atexit_log.record(kSlot558VectorDtorRva, &v)', 'objects.owner = 0;', 'registry.atexit_log.record(kSlot558OwnerDtorRva, &objects.owner)'],
        'test_tokens': ['kSlot558Rva == 0x428a0', 'section_alone()', 'check_atexit(creg, 6, 0x41910, &o.owner)'],
        'cmake_tokens': ['core/ctor_slot_558_428a0.cpp', 'add_executable(test_ctor_slot_558 tests/test_ctor_slot_558.cpp)', 'add_test(NAME ctor_slot_558 COMMAND test_ctor_slot_558)'],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, '5 zeroed cells + inline vector + owner cell, 7 __cxa_atexit registrations (0x418c0 x5, 0x41900, 0x41910)'))
