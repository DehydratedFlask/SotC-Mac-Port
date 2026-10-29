#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 543 (RVA 0xfae50, 289 bytes), lane M5.

Named delegate entry `UpdateMaterialProperties` (handler tag 0xf9520, invoker 0x4cca0) queued by 0x4b560, atexit 0x4b660.

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
    "slot": 543,
    "base": 0xfae50,
    "size": 289,
    "body_sha256": "c9d7502a7afee13fae64ff43dcd9df2750155040e45dba78fd9fbd04e0cfc7cc",
    "pad": "909090909090909090909090909090",
    "callees": ['name_view_f2f0', 'entry_register_4b560', 'entry_dtor_4b660', 'delegate_invoker_4cca0', 'release_f590'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0xfa5022, b'UpdateMaterialProperties'),
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0xfae5f, 7, 3, 0x15fe518),
        ("rip", 0xfae66, 7, 3, 0xfa5022),
        ("call", 0xfae7c, 0xf2f0),
        ("rip", 0xfae81, 7, 3, 0x2edef10),
        ("rip", 0xfae93, 8, 4, 0x15fe800),
        ("rip", 0xfae9f, 7, 3, 0xf9520),
        ("imm", 0xfaeaa, "c60000"),
        ("imm", 0xfaed8, "eb21"),
        ("rip", 0xfaeda, 8, 4, 0x15fe800),
        ("imm", 0xfaf04, "48c7403800000000"),
        ("rip", 0xfaf2a, 7, 3, 0x2edef10),
        ("call", 0xfaf34, 0x4b560),
        ("call", 0xfaf3e, 0xf590),
        ("rip", 0xfaf43, 7, 3, 0x4b660),
        ("rip", 0xfaf4a, 7, 3, 0x1604218),
        ("call", 0xfaf54, 0xf8f390),
        ("call", 0xfaf6c, 0xf8f3a0),
    ],
    "elf_extra": [lib.app_entry_extra],
    "native": {'header': 'native/core/ctor_slot_543_fae50.hpp',
     'source': 'native/core/ctor_slot_543_fae50.cpp',
     'test': 'native/tests/test_ctor_slot_543.cpp',
     'cmake': 'native/lanes/m5.cmake',
     'consts': {'kSlot543Rva': 1027664, 'kSlot543Size': 289, 'kSlot543ObjectRva': 49147664},
     'header_tokens': ['kSlot543HandlerTag = 0xf9520'],
     'source_order': ['l2::literal_view(kSlot543Name)', 'lane_m5::init_named_delegate_entry(objects.entry, spec, queue, concmd.atexit_log)'],
     'test_tokens': ['kSlot543Rva == 0xfae50', 'section_alone()', 'section_failures()', 'section_with_neighbour()'],
     'cmake_tokens': ['core/ctor_slot_543_fae50.cpp',
                      'add_executable(test_ctor_slot_543 tests/test_ctor_slot_543.cpp)',
                      'add_test(NAME ctor_slot_543 COMMAND test_ctor_slot_543)'],
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
    raise SystemExit(lib.main(SPEC, 'Named delegate entry `UpdateMaterialProperties` (handler tag 0xf9520, invoker 0x4cca0) queued by 0x4b560, atexit 0x4b660.'))
