#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 557 (RVA 0x47490, 296 bytes), lane M6.

The console command `mem` (description `Dump memory statistics`, handler tag 0x465a0 never called, flags 2,
atexit 0x4b0), built exactly like the slot 306 / 343 commands.

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
    "slot": 557,
    "base": 0x47490,
    "size": 296,
    "body_sha256": "b121d4d656b6ed28641cfc2cc8a4c10db72fdfa4ec25f9bd8356254e286d4163",
    "pad": "9090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'cmd_dtor', 'release_f590'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0xf98eb8, b'mem'),
        (0xf98ebc, b'Dump memory statistics'),
    ],
    "lengths": [
        ('Dump memory statistics', 0x474f9, "48c744243816000000"),
    ],
    "pins": [
        ("imm", 0x47491, "4889e5"),
        ("imm", 0x47499, "4883e4e0"),
        ("imm", 0x4749d, "4881ec80000000"),
        ("rip", 0x474a4, 7, 3, 0x15fe518),
        ("imm", 0x474ab, "4c8d742450"),
        ("rip", 0x474b0, 7, 3, 0xf98eb8),
        ("imm", 0x474b7, "31f6"),
        ("imm", 0x474b9, "4c89f7"),
        ("imm", 0x474bc, "498b07"),
        ("imm", 0x474bf, "4889442470"),
        ("call", 0x474c4, 0xf2f0),
        ("rip", 0x474c9, 7, 3, 0x2ea8868),
        ("rip", 0x474d0, 8, 4, 0x15fe5e0),
        ("rip", 0x474d8, 7, 3, 0xf98ebc),
        ("imm", 0x474df, "c5f857c0"),
        ("imm", 0x474e3, "488d542430"),
        ("imm", 0x474e8, "b902000000"),
        ("imm", 0x474ed, "4c89f6"),
        ("imm", 0x474f0, "4889442430"),
        ("imm", 0x474f5, "488d0424"),
        ("imm", 0x474f9, "48c744243816000000"),
        ("imm", 0x47502, "c5f811442440"),
        ("imm", 0x47508, "48890424"),
        ("rip", 0x4750c, 7, 3, 0x465a0),
        ("imm", 0x47513, "4889df"),
        ("imm", 0x47516, "c5fa7f4c2408"),
        ("imm", 0x4751c, "4889442418"),
        ("call", 0x47521, 0x20af0),
        ("rip", 0x47526, 7, 3, 0x1594dc0),
        ("imm", 0x4752d, "c5fc100424"),
        ("imm", 0x47532, "488d4810"),
        ("imm", 0x47536, "48890b"),
        ("imm", 0x47539, "c5fc114360"),
        ("imm", 0x4753e, "c4e37d19c001"),
        ("imm", 0x47544, "c4e3f916c101"),
        ("imm", 0x4754a, "4885c9"),
        ("imm", 0x4754d, "7408"),
        ("imm", 0x4754f, "488d4b60"),
        ("imm", 0x47553, "48894b60"),
        ("imm", 0x47557, "c5f857c0"),
        ("imm", 0x4755b, "4889df"),
        ("imm", 0x4755e, "c5f8118380000000"),
        ("imm", 0x47566, "48c7839000000000000000"),
        ("imm", 0x47571, "ff5020"),
        ("imm", 0x47574, "488d7c2430"),
        ("call", 0x47579, 0xf590),
        ("imm", 0x4757e, "488d7c2450"),
        ("call", 0x47583, 0xf590),
        ("rip", 0x47588, 7, 3, 0x4b0),
        ("rip", 0x4758f, 7, 3, 0x1604218),
        ("imm", 0x47596, "4889de"),
        ("call", 0x47599, 0xf8f390),
        ("imm", 0x4759e, "498b07"),
        ("imm", 0x475a1, "483b442470"),
        ("imm", 0x475a6, "750b"),
        ("imm", 0x475a8, "488d65e8"),
        ("call", 0x475b3, 0xf8f3a0),
    ],
    "m6_callees": {
        "handler_tag_465a0": (0x465a0, 0x76, "79fb2ac49c91e10487079d5bcae30d0a9148c448aa3fdc0a58846f090bbd790e"),
    },
    "m6_extras": ['fcmd_vtable'],
    "native": {
        'header': 'native/core/ctor_slot_557_47490.hpp',
        'source': 'native/core/ctor_slot_557_47490.cpp',
        'test': 'native/tests/test_ctor_slot_557.cpp',
        'cmake': 'native/lanes/m6.cmake',
        'consts': {'kSlot557Rva': 291984, 'kSlot557Size': 296, 'kSlot557CommandDescriptionLength': 22, 'kSlot557HandlerTag': 288160},
        'header_tokens': ['kSlot557CommandDescription[] = "Dump memory statistics"'],
        'source_order': ['init_fconcmd_object(objects.command, spec, concmd)'],
        'test_tokens': ['kSlot557Rva == 0x47490', 'section_alone()', 'section_failures()', 'check_atexit(creg, 0, 0x4b0, &o.command)'],
        'cmake_tokens': ['core/ctor_slot_557_47490.cpp', 'add_executable(test_ctor_slot_557 tests/test_ctor_slot_557.cpp)', 'add_test(NAME ctor_slot_557 COMMAND test_ctor_slot_557)'],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'console command `mem` (flags 2, handler tag 0x465a0, atexit 0x4b0)'))
