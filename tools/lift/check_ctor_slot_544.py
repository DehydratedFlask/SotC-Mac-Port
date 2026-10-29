#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 544 (RVA 0xd1b20, 273 bytes), lane M6.

memset(0x2ed3f10, 0, 0xae40) and the 13-record shadow-pass name table at 0x2eded90 (ten records written, three left
untouched, terminator qword 0). No atexit, no console objects.

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
    "slot": 544,
    "base": 0xd1b20,
    "size": 273,
    "body_sha256": "136ea41d5517d34c79635e86f3882b981d41f05def4952fb8212a99ffe4f648d",
    "pad": "909090909090909090909090909090",
    "callees": [],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0xfa126f, b'SpotShadow0'),
        (0xfa127b, b'SpotShadow1'),
        (0xfa1287, b'SpotShadow2'),
        (0xfa1293, b'SpotShadow3'),
        (0xfa145f, b'ShadowCascade0'),
        (0xfa146e, b'ShadowCascade1'),
        (0xfa147d, b'ShadowCascade2'),
        (0xfa148c, b'ShadowCascade3'),
        (0xfa149b, b'ShadowCascade4(SuperClose)'),
        (0xfa14b6, b'EntireFrame'),
    ],
    "lengths": [
    ],
    "pins": [
        ("imm", 0xd1b21, "4889e5"),
        ("rip", 0xd1b24, 7, 3, 0x2ed3f10),
        ("imm", 0xd1b2b, "31f6"),
        ("imm", 0xd1b2d, "ba40ae0000"),
        ("call", 0xd1b32, 0xf8f4f0),
        ("rip", 0xd1b37, 7, 3, 0xfa126f),
        ("rip", 0xd1b3e, 7, 3, 0xfa127b),
        ("rip", 0xd1b45, 10, 2, 0x2eded90),
        ("imm", 0xd1b4b, "07000000"),
        ("rip", 0xd1b4f, 7, 3, 0xfa1287),
        ("rip", 0xd1b56, 10, 2, 0x2edee00),
        ("imm", 0xd1b5c, "0c000000"),
        ("rip", 0xd1b60, 7, 3, 0x2edee50),
        ("imm", 0xd1b67, "31f6"),
        ("rip", 0xd1b69, 7, 3, 0x2eded98),
        ("rip", 0xd1b70, 10, 2, 0x2ededa0),
        ("imm", 0xd1b76, "08000000"),
        ("rip", 0xd1b7a, 7, 3, 0x2ededa8),
        ("rip", 0xd1b81, 7, 3, 0xfa1293),
        ("rip", 0xd1b88, 10, 2, 0x2ededb0),
        ("imm", 0xd1b8e, "09000000"),
        ("rip", 0xd1b92, 7, 3, 0x2ededb8),
        ("rip", 0xd1b99, 10, 2, 0x2ededc0),
        ("imm", 0xd1b9f, "0a000000"),
        ("rip", 0xd1ba3, 7, 3, 0xfa145f),
        ("rip", 0xd1baa, 7, 3, 0x2ededc8),
        ("rip", 0xd1bb1, 7, 3, 0xfa146e),
        ("rip", 0xd1bb8, 7, 3, 0x2edee08),
        ("rip", 0xd1bbf, 10, 2, 0x2edee10),
        ("imm", 0xd1bc5, "0d000000"),
        ("rip", 0xd1bc9, 7, 3, 0xfa147d),
        ("rip", 0xd1bd0, 7, 3, 0x2edee18),
        ("rip", 0xd1bd7, 7, 3, 0xfa148c),
        ("rip", 0xd1bde, 10, 2, 0x2edee20),
        ("imm", 0xd1be4, "0e000000"),
        ("rip", 0xd1be8, 7, 3, 0x2edee28),
        ("rip", 0xd1bef, 10, 2, 0x2edee30),
        ("imm", 0xd1bf5, "0f000000"),
        ("rip", 0xd1bf9, 7, 3, 0xfa149b),
        ("rip", 0xd1c00, 7, 3, 0x2edee38),
        ("rip", 0xd1c07, 7, 3, 0xfa14b6),
        ("rip", 0xd1c0e, 10, 2, 0x2edee40),
        ("imm", 0xd1c14, "10000000"),
        ("rip", 0xd1c18, 7, 3, 0x2edee48),
        ("call", 0xd1c1f, 0xc1c70),
        ("rip", 0xd1c24, 11, 3, 0x2edee60),
        ("imm", 0xd1c2b, "00000000"),
    ],
    "m6_callees": {
        "entry_writer_c1c70": (0xc1c70, 0x7, "ce1839d005df30c632573ba3f4aa75d80fdc8d2c5099b6dbcf4f5c5e103616bf"),
        "memset_thunk_f8f4f0": (0xf8f4f0, 0x6, "92b109514de9fe8fff9827cf5f9bb4d2fa347397a50bd76a8b6c644d1992855b"),
    },
    "m6_extras": ['memset_thunk'],
    "native": {
        'header': 'native/core/ctor_slot_544_d1b20.hpp',
        'source': 'native/core/ctor_slot_544_d1b20.cpp',
        'test': 'native/tests/test_ctor_slot_544.cpp',
        'cmake': 'native/lanes/m6.cmake',
        'consts': {'kSlot544Rva': 858912, 'kSlot544Size': 273, 'kSlot544BlockRva': 49102608, 'kSlot544BlockSize': 44608, 'kSlot544TableRva': 49147280, 'kSlot544TerminatorRva': 49147488, 'kSlot544EntryWriterRva': 793712, 'kSlot544TableEntries': 13},
        'header_tokens': ['Slot544Entry entries[kSlot544TableEntries]', 'offsetof(Slot544Objects, terminator) - offsetof(Slot544Objects, entries) == 0xd0'],
        'source_order': ['std::memset(objects.block.data(), 0, objects.block.size())', 'set_entry(e[0], 7, kSlot544Name_SpotShadow0)', 'set_entry(e[1], 8, kSlot544Name_SpotShadow1)', 'set_entry(e[2], 9, kSlot544Name_SpotShadow2)', 'set_entry(e[3], 10, kSlot544Name_SpotShadow3)', 'set_entry(e[7], 0xc, kSlot544Name_ShadowCascade0)', 'set_entry(e[8], 0xd, kSlot544Name_ShadowCascade1)', 'set_entry(e[9], 0xe, kSlot544Name_ShadowCascade2)', 'set_entry(e[10], 0xf, kSlot544Name_ShadowCascade3)', 'set_entry(e[11], 0x10, kSlot544Name_ShadowCascade4)', 'set_entry(e[12], 0, kSlot544Name_EntireFrame)', 'objects.terminator = 0'],
        'test_tokens': ['kSlot544Rva == 0xd1b20', 'kSlot544TerminatorRva == kSlot544TableRva + 0xd0', 'section_alone()', 'for (std::size_t i = 4; i <= 6; ++i)', '{12, 0, "EntireFrame"}'],
        'cmake_tokens': ['core/ctor_slot_544_d1b20.cpp', 'add_executable(test_ctor_slot_544 tests/test_ctor_slot_544.cpp)', 'add_test(NAME ctor_slot_544 COMMAND test_ctor_slot_544)'],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'memset of a 0xae40 block + ten {id, name} table records (SpotShadow0..3, ShadowCascade0..4, EntireFrame)'))
