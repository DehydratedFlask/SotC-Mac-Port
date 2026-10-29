#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 492 (RVA 0x278640, 289 bytes), lane M5.

Named delegate entry `RequestTelemetryChannels` (handler tag 0x278300, invoker 0x4cca0) queued by 0x4b560, atexit 0x4b660.

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
    "slot": 492,
    "base": 0x278640,
    "size": 289,
    "body_sha256": "537d834bab62cb19874fee66dd3cd72e4e0f7fe7b115ed4ff2820116cddfe17f",
    "pad": "909090909090909090909090909090",
    "callees": ['name_view_f2f0', 'entry_register_4b560', 'entry_dtor_4b660', 'delegate_invoker_4cca0', 'release_f590'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0xfc5382, b'RequestTelemetryChannels'),
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x27864f, 7, 3, 0x15fe518),
        ("rip", 0x278656, 7, 3, 0xfc5382),
        ("call", 0x27866c, 0xf2f0),
        ("rip", 0x278671, 7, 3, 0x2eef4f0),
        ("rip", 0x278683, 8, 4, 0x15fe800),
        ("rip", 0x27868f, 7, 3, 0x278300),
        ("imm", 0x27869a, "c60000"),
        ("imm", 0x2786c8, "eb21"),
        ("rip", 0x2786ca, 8, 4, 0x15fe800),
        ("imm", 0x2786f4, "48c7403800000000"),
        ("rip", 0x27871a, 7, 3, 0x2eef4f0),
        ("call", 0x278724, 0x4b560),
        ("call", 0x27872e, 0xf590),
        ("rip", 0x278733, 7, 3, 0x4b660),
        ("rip", 0x27873a, 7, 3, 0x1604218),
        ("call", 0x278744, 0xf8f390),
        ("call", 0x27875c, 0xf8f3a0),
    ],
    "elf_extra": [lib.app_entry_extra],
    "native": {'header': 'native/core/ctor_slot_492_278640.hpp',
     'source': 'native/core/ctor_slot_492_278640.cpp',
     'test': 'native/tests/test_ctor_slot_492.cpp',
     'cmake': 'native/lanes/m5.cmake',
     'consts': {'kSlot492Rva': 2590272, 'kSlot492Size': 289, 'kSlot492ObjectRva': 49214704},
     'header_tokens': ['kSlot492HandlerTag = 0x278300'],
     'source_order': ['l2::literal_view(kSlot492Name)', 'lane_m5::init_named_delegate_entry(objects.entry, spec, queue, concmd.atexit_log)'],
     'test_tokens': ['kSlot492Rva == 0x278640', 'section_alone()', 'section_failures()', 'section_with_neighbour()'],
     'cmake_tokens': ['core/ctor_slot_492_278640.cpp',
                      'add_executable(test_ctor_slot_492 tests/test_ctor_slot_492.cpp)',
                      'add_test(NAME ctor_slot_492 COMMAND test_ctor_slot_492)'],
     'shapes_h': 'native/core/lane_m5_app_entry.hpp',
     'shapes_c': 'native/core/lane_m5_app_entry.cpp',
     'shapes_tokens': ['if (entry.registered != 0) return out;',
                       'entry.registered = 1;',
                       'entry.next = queue.head;',
                       'queue.head = &entry;',
                       'entry.delegate.fn = kDelegateInvokerRva;',
                       'if (spec.handler_tag == 0) return out;',
                       'register_entry_4b560(entry, queue)',
                       'record(kEntryDtorRva, &entry)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Named delegate entry `RequestTelemetryChannels` (handler tag 0x278300, invoker 0x4cca0) queued by 0x4b560, atexit 0x4b660.'))
