#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 535 (RVA 0x138e20, 96 bytes), lane M5.

Critical section 0x3b310 (atexit 0x3b380) + 0x20 B object with [+8,+0x20) zeroed (atexit 0x138370); host RecursiveMutex stand-in.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and indirect call of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings and the hash of every shared callee. A mutation self-test flips the
first/last byte of every pin and perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lane_m5_check as lib  # noqa: E402

SPEC = {
    "slot": 535,
    "base": 0x138e20,
    "size": 96,
    "body_sha256": "f5cca62b7739ad3c693c625f6fe01368d3bfbad45fdb262028c3129fe7e7fe08",
    "pad": "554889e5",
    "callees": ['critsec_ctor_3b310', 'critsec_dtor_3b380', 'buffer_dtor_138370'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x138e27, 7, 3, 0x2ee14f0),
        ("call", 0x138e31, 0x3b310),
        ("rip", 0x138e36, 7, 3, 0x1604218),
        ("rip", 0x138e3d, 7, 3, 0x3b380),
        ("call", 0x138e4a, 0xf8f390),
        ("rip", 0x138e53, 7, 3, 0x2ee14f8),
        ("rip", 0x138e5a, 7, 3, 0x138370),
        ("rip", 0x138e64, 8, 4, 0x2ee1500),
        ("rip", 0x138e6c, 11, 3, 0x2ee1510),
        ("jmp", 0x138e7b, 0xf8f390),
    ],
    "elf_extra": [lib.critsec_extra],
    "native": {'header': 'native/core/ctor_slot_535_138e20.hpp',
     'source': 'native/core/ctor_slot_535_138e20.cpp',
     'test': 'native/tests/test_ctor_slot_535.cpp',
     'cmake': 'native/lanes/m5.cmake',
     'consts': {'kLaneM5Slot535Rva': 1281568,
                'kLaneM5Slot535Size': 96,
                'kLaneM5Slot535MutexObjectRva': 49157360,
                'kLaneM5Slot535ObjectRva': 49157368,
                'kLaneM5Slot535MutexDtorRva': 242560,
                'kLaneM5Slot535ObjectDtorRva': 1278832},
     'header_tokens': [],
     'source_order': ['objects.mutex.initialize()',
                      'log.record(kLaneM5Slot535MutexDtorRva, &objects.mutex)',
                      'std::memset(objects.object + 0x08, 0, 0x10)',
                      'std::memset(objects.object + 0x18, 0, 0x08)',
                      'log.record(kLaneM5Slot535ObjectDtorRva, objects.object)'],
     'test_tokens': ['kLaneM5Slot535Rva == 0x138e20', 'section_alone()', 'section_failures()'],
     'cmake_tokens': ['core/ctor_slot_535_138e20.cpp',
                      'add_executable(test_ctor_slot_535 tests/test_ctor_slot_535.cpp)',
                      'add_test(NAME ctor_slot_535 COMMAND test_ctor_slot_535)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Critical section 0x3b310 (atexit 0x3b380) + 0x20 B object with [+8,+0x20) zeroed (atexit 0x138370); host RecursiveMutex stand-in.'))
