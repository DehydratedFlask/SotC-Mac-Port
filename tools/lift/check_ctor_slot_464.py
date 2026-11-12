#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 464 (RVA 0x3db070, 697 bytes), batch 14, a bool plus records.

Generated from the ELF by tools/lift/startup_gen (a static register tracker over the pinned ELF; it reads bytes and never executes them). Every constructor row is pinned operand by operand: object addresses, name string
addresses and length immediates, flags, init / range immediates, the caller functor tags and the __cxa_atexit records. The 5 named records go through the registrar 0xa81700 (byte-pinned in check_helper_a81700.py; whole-body hashes here). The group destructor 0x3d9b40 unregisters the block's records last first (shape checked, hashed, never run). Vtables, functor tags and destructors are IDENTITY TAGS (recorded, never run). What reads the variables at run time is NOT established (readings are inference from the strings).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 464,
    "base": 0x3db070,
    "size": 697,
    "body_sha256": "997d66b8352c2407ec4d41a88ce82328509aaba9fde9701380b6b40d3cfe5ded",
    "pad": "90909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'release_f590', 'bool_dtor', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15b1558],
    "float_tags": [],
    "strings": [
        (0xfe87c8, b'network_printHiearchy'),
        (0xfe8a69, b'Server: network objects'),
        (0xfe8a81, b'Server: objects in view'),
        (0xfe8a99, b'Server: object gather time'),
        (0xfe8ab4, b'Server: time to add new objects'),
        (0xfe8ad4, b'Server: new object count'),
        (0xfe8b1c, b''),
    ],
    "lengths": [
        ('network_printHiearchy', 0x3db0d5, "48c7459815000000"),
        ('Server: network objects', 0x3db1c2, "48c745b817000000"),
        ('Server: objects in view', 0x3db200, "48c7459817000000"),
        ('Server: object gather time', 0x3db244, "48c78578ffffff1a000000"),
        ('Server: time to add new objects', 0x3db28b, "48c78558ffffff1f000000"),
        ('Server: new object count', 0x3db2d5, "48c78538ffffff18000000"),
    ],
    "pins": [
        ("rip", 0x3db084, 7, 3, 0x15fe518),
        ("rip", 0x3db08b, 7, 3, 0x1594208),
        ("rip", 0x3db092, 7, 3, 0x2ef70b0),
        ("rip", 0x3db099, 7, 3, 0xfe87c8),
        ("rip", 0x3db0a0, 7, 3, 0xfe8b1c),
        ("imm", 0x3db0b3, "b901000000"),
        ("imm", 0x3db0bf, "4883c310"),
        ("imm", 0x3db0d5, "48c7459815000000"),
        ("rip", 0x3db0e7, 7, 3, 0x2ef70b0),
        ("imm", 0x3db0ee, "48c745c800000000"),
        ("call", 0x3db0f6, 0x20af0),
        ("rip", 0x3db0fb, 7, 3, 0x15940a0),
        ("rip", 0x3db105, 7, 3, 0x2ef7170),
        ("rip", 0x3db110, 7, 3, 0x15b1558),
        ("imm", 0x3db117, "4883c010"),
        ("rip", 0x3db11b, 7, 3, 0x2ef70b0),
        ("rip", 0x3db122, 7, 3, 0x2ef7110),
        ("rip", 0x3db129, 10, 2, 0x2ef7119),
        ("imm", 0x3db129, "c705e6bfb10200000000"),
        ("rip", 0x3db133, 8, 4, 0x2ef7120),
        ("rip", 0x3db13b, 11, 3, 0x2ef7150),
        ("imm", 0x3db13b, "48c7050ac0b10200000000"),
        ("rip", 0x3db146, 7, 3, 0x2ef7160),
        ("rip", 0x3db14d, 11, 3, 0x2ef7168),
        ("imm", 0x3db14d, "48c70510c0b10200000000"),
        ("call", 0x3db158, 0xf590),
        ("rip", 0x3db160, 7, 3, 0x2ef70b0),
        ("rip", 0x3db167, 7, 2, 0x2ef7170),
        ("imm", 0x3db167, "c60502c0b10200"),
        ("rip", 0x3db16e, 7, 2, 0x2ef7118),
        ("imm", 0x3db16e, "c605a3bfb10200"),
        ("call", 0x3db175, 0x20d60),
        ("call", 0x3db17d, 0xf590),
        ("rip", 0x3db182, 7, 3, 0x1604218),
        ("rip", 0x3db189, 7, 3, 0x20b0),
        ("call", 0x3db196, 0xf8f390),
        ("rip", 0x3db19b, 7, 3, 0x2ef7178),
        ("rip", 0x3db1a6, 7, 3, 0xfe8a69),
        ("imm", 0x3db1b1, "ba01000000"),
        ("imm", 0x3db1c2, "48c745b817000000"),
        ("call", 0x3db1cf, 0xa81700),
        ("call", 0x3db1d7, 0xf590),
        ("rip", 0x3db1e0, 7, 3, 0xfe8a81),
        ("rip", 0x3db1eb, 7, 3, 0x2ef71b0),
        ("imm", 0x3db1f2, "ba01000000"),
        ("imm", 0x3db200, "48c7459817000000"),
        ("call", 0x3db20d, 0xa81700),
        ("call", 0x3db215, 0xf590),
        ("rip", 0x3db221, 7, 3, 0xfe8a99),
        ("rip", 0x3db22c, 7, 3, 0x2ef71e8),
        ("imm", 0x3db233, "ba01000000"),
        ("imm", 0x3db244, "48c78578ffffff1a000000"),
        ("call", 0x3db254, 0xa81700),
        ("call", 0x3db25c, 0xf590),
        ("rip", 0x3db268, 7, 3, 0xfe8ab4),
        ("rip", 0x3db273, 7, 3, 0x2ef7220),
        ("imm", 0x3db27a, "ba01000000"),
        ("imm", 0x3db28b, "48c78558ffffff1f000000"),
        ("call", 0x3db29e, 0xa81700),
        ("call", 0x3db2a6, 0xf590),
        ("rip", 0x3db2b2, 7, 3, 0xfe8ad4),
        ("rip", 0x3db2bd, 7, 3, 0x2ef7258),
        ("imm", 0x3db2c4, "ba01000000"),
        ("imm", 0x3db2d5, "48c78538ffffff18000000"),
        ("call", 0x3db2e8, 0xa81700),
        ("call", 0x3db2f0, 0xf590),
        ("rip", 0x3db2f5, 7, 3, 0x3d9b40),
        ("call", 0x3db302, 0xf8f390),
        ("call", 0x3db323, 0xf8f3a0),
    ],
    "m6_callees": {
        'named_record_ctor_a81700': (0xa81700, 190, "d1cd3b80f06fb1259fd898a474ba53bccf44eca8b9bc0c508b8df3b2e9c39411"),
        'record_registry_push_a81840': (0xa81840, 265, "5b5f3c60b45197d5521030ce80c1de96ed4dc5540a6d1863798c4a2a6d7d5283"),
        'vec_reserve_a82050': (0xa82050, 245, "def6d82b0c831bb693039b2cf3b92f338ee2c41b185148965f3d9ac201cd9831"),
        'record_registry_ctor_a81b50': (0xa81b50, 34, "99b8398a9607e7e3870e3157958ca1ac8a5903b1dfad1cd959d2f70f43bd808f"),
        'records_dtor_3d9b40': (0x3d9b40, 65, "94ee046b1129a309bb2304f364195d6b56878a051f7a00a149ddad0dbac51d80"),
        'record_unregister_a81950': (0xa81950, 147, "db1c5f59ff06d01e4ac087fcd21bf8ae1c489fe0d985730f9d360fbbbe8d0845"),
        'record_registry_dtor_a81b80': (0xa81b80, 138, "0bb33a6d2795d0a61b221e9012b6ee75e956c26545803d0c63399508b843d64f"),
    },
    "m6_extras": ['bp_atexit_imports', 'bp_object_vtables'],
    "native": {'header': 'native/core/ctor_slot_464_3db070.hpp', 'source': 'native/core/ctor_slot_464_3db070.cpp', 'test': 'native/tests/test_ctor_slot_464.cpp', 'consts': {'kSlot464Rva': 4042864, 'kSlot464Size': 697, 'kSlot464BoolFnTag': 22746456, 'kSlot464FloatFnTag': 0, 'kSlot464ItemFlags': 48, 'kSlot464VarFlags': 1, 'kSlot464BoolCount': 1, 'kSlot464FloatCount': 0, 'kSlot464UFloatFnTag': 0, 'kSlot464UFloatCount': 0, 'kSlot464IntFnTag': 0, 'kSlot464IntCount': 0, 'kSlot464LocalIntCount': 0, 'kSlot464UIntFnTag': 0, 'kSlot464UIntCount': 0, 'kSlot464CmdCount': 0, 'kSlot464StrVarCount': 0, 'kSlot464RowCount': 2, 'kSlot464StringCount': 0, 'kSlot464SliderCount': 0, 'kSlot464Object0Rva': 49246384, 'kSlot464CellCount': 0, 'kSlot464HolderCount': 0, 'kSlot464SentinelCount': 0, 'kSlot464RecsCount': 1, 'kSlot464RecordCount': 5, 'kSlot464RecsDtorRva': 4037440},
        'header_tokens': ['"network_printHiearchy"', 'std::size_t kSlot464BoolNameLengths[kSlot464BoolCount] = {0x15}', 'std::uint8_t kSlot464BoolValues[kSlot464BoolCount] = {0}', 'std::int32_t kSlot464BoolDesc[kSlot464BoolCount] = {-1}', 'std::uint8_t kSlot464BoolEarly[kSlot464BoolCount] = {1}', 'std::size_t kSlot464RecNameLength[kSlot464RecordCount] = {0x17, 0x17, 0x1a, 0x1f, 0x18}', 'std::uint32_t kSlot464RecKind[kSlot464RecordCount] = {1, 1, 1, 1, 1}', 'std::uint8_t kSlot464RecFlag[kSlot464RecordCount] = {0, 0, 0, 0, 0}', 'IDENTITY TAGS'], 'source_order': ['lane_m6::init_bool_var_literal(var, ', 'register_named_record_a81700(*cell, recs.rec[n], view, kSlot464RecKind[n], kSlot464RecFlag[n], alloc)', 'concmd.atexit_log.record(kRecordRegistryDtorRva, &cell->registry)', 'concmd.atexit_log.record(kSlot464RecsDtorRva, &recs)', 'objects.r00', 'objects.r01'], 'test_tokens': ['kSlot464Rva == 0x3db070 && kSlot464Size == 697', 'section_constants()', 'section_alone()', 'section_failures()', 'CHECK(items == 0);', 'creg.atexit_log.size() == kSlot464RowCount - cells + kSlot464RecsCount);', 'r.rows_done == 0']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'one bool, five named records and a group destructor'))
