#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 415 (RVA 0x578b60, 331 bytes), lane M2.

Unbounded int variable `isysLinkAllocMax` through the 0x3a980 worker (flags 8, value/default 0, functor table 0x15cb128, atexit 0x18a60).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and register store of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings, the functor vtables and the hash of every shared callee. A mutation
self-test flips the first/last byte of every pin and perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

from slot_pin_util import require  # noqa: E402

# Int-variable callees (hashes identical to tools/lift/slotfam_check.py) and the int functor/class tables.
lib.COMMON_CALLEES.update({
    "int_var_worker": (0x3A980, 0x142, "2cbfc2763abb6506a59551b902dd6098325889e5e520d842c0e93da7f43498ca"),
    "int_dtor": (0x18A60, 0x36, "4de4f3993b02c84c9c20824690e8ab9d4f7a1b84babfa6cbe0d89c9dd558b50b"),
    "int_slot3": (0xBF70, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "int_slot6": (0xBFC0, 0x6, "7140f35dee6220b79b12aecc27acf5105bf3b77d1588e89fce345de7c16c72b7"),
    "int_fn_copy": (0xC720, 0x14, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
    "int_fn_clone": (0xC740, 0x14, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
    "int_fn_move": (0xC760, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "int_fn_destroy": (0xC770, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
})
INT_FN_BODIES = (0xC720, 0xC740, 0xC760, 0xC770)
INT_FN_TABLES = (0x15CB128, 0x15CE8C0, 0x1595660, 0x15944A0)   # caller functor (this slot), second lifted table, worker seed, class tag
_base_run_elf_checks = lib.run_elf_checks


def run_int_checks(e, spec):
    _base_run_elf_checks(e, spec)
    for tag in INT_FN_TABLES:
        for i, body in enumerate(INT_FN_BODIES):
            require(e.reloc_target(tag + 8 * i) == body, f"int functor table {tag:#x} slot {i}")
    require(e.rip(0xC724, 7, 3) == 0x15944A0, "int functor copy stores class tag 0x15944a0")
    require(e.rip(0xC740, 7, 3) == 0x15944A0, "int functor clone stores class tag 0x15944a0")
    require(e.reloc_target(0x1594C08) == 0x18A60, "int class slot 0 = 0x18a60")
    require(e.reloc_target(0x1594C08 + 0x10) == 0x20D60, "int class slot 2 = 0x20d60")
    require(e.reloc_target(0x1594C08 + 0x18) == 0xBF70, "int class slot 3 = 0xbf70 (returns 1)")
    require(e.reloc_target(0x1594C08 + 0x30) == 0xBFC0, "int class slot 6 = 0xbfc0 (returns 2)")


lib.run_elf_checks = run_int_checks


SPEC = {
    "slot": 415,
    "base": 0x578b60,
    "size": 331,
    "body_sha256": "46b1a3bc0c00e7f50bf1b0e07eac7d8e23dd5f8baed27ae6d89e390b17eaa3ab",
    "pad": "9090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'release_f590', 'int_var_worker', 'int_dtor', 'int_slot3', 'int_slot6', 'int_fn_copy', 'int_fn_clone', 'int_fn_move', 'int_fn_destroy'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0x1023864, b'isysLinkAllocMax'),
        (0x1023bd2, b''),
    ],
    "lengths": [
        ('isysLinkAllocMax', 0x578bf5, "48c744240810000000"),
    ],
    "pins": [
        ("rip", 0x578b78, 7, 3, 0x15fe518),
        ("rip", 0x578b7f, 7, 3, 0x1594bf8),
        ("rip", 0x578b86, 7, 3, 0x2f295e8),
        ("rip", 0x578b8d, 7, 3, 0x1023864),
        ("rip", 0x578b9d, 7, 3, 0x15cb128),
        ("imm", 0x578bb1, "b908000000"),
        ("imm", 0x578bc1, "4983c510"),
        ("imm", 0x578bc5, "4889df"),
        ("rip", 0x578bd4, 7, 3, 0x1023bd2),
        ("imm", 0x578bdb, "48c744247000000000"),
        ("imm", 0x578bf5, "48c744240810000000"),
        ("imm", 0x578c04, "4c892b"),
        ("imm", 0x578c07, "48c784248800000000000000"),
        ("imm", 0x578c25, "48c744243800000000"),
        ("call", 0x578c2e, 0x3a980),
        ("call", 0x578c36, 0xf590),
        ("imm", 0x578c46, "ff5018"),
        ("imm", 0x578c49, "4889df"),
        ("imm", 0x578c4c, "4c892b"),
        ("imm", 0x578c4f, "c783d000000000000000"),
        ("imm", 0x578c59, "c7436800000000"),
        ("call", 0x578c60, 0x20d60),
        ("call", 0x578c68, 0xf590),
        ("rip", 0x578c6d, 7, 3, 0x18a60),
        ("rip", 0x578c74, 7, 3, 0x1604218),
        ("call", 0x578c7e, 0xf8f390),
        ("rip", 0x578c83, 7, 3, 0x15fe518),
        ("call", 0x578ca6, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_415_578b60.hpp',
     'source': 'native/core/ctor_slot_415_578b60.cpp',
     'test': 'native/tests/test_ctor_slot_415.cpp',
     'cmake': 'native/lanes/m2.cmake',
     'shapes_h': 'native/core/lane_m2_shapes.hpp',
     'shapes_c': 'native/core/lane_m2_shapes.cpp',
     'consts': {'kSlot415Rva': 5737312, 'kSlot415Size': 331},
     'header_tokens': ['kSlot415ArgFnTable = 0x15cb128', 'kSlot415CtorFlags = 8'],
     'source_order': ['lane_m2::init_unbounded_int_var_site(objects.var, kSlot415Spec, concmd)'],
     'shapes_tokens': ['table == kIntArgFnTable_15cb128', 'registry.atexit_log.record(0x18a60, &var)'],
     'test_tokens': ['kSlot415Rva == 0x578b60', 'section_alone()', 'section_failures()', 'section_unknown_functor_refused()'],
     'cmake_tokens': ['core/ctor_slot_415_578b60.cpp',
                      'add_executable(test_ctor_slot_415 tests/test_ctor_slot_415.cpp)',
                      'add_test(NAME ctor_slot_415 COMMAND test_ctor_slot_415)',
                      'core/lane_m2_shapes.cpp']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Unbounded int variable `isysLinkAllocMax` through the 0x3a980 worker (flags 8, value/default 0, functor 0x15cb128).'))
