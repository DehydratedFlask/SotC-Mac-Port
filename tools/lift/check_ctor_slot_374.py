#!/usr/bin/env python3
"""ELF tie for startup constructor index 374 (RVA 0x701230, 537 B): one `{name, fnv1a(name)}` record,
then one bounded float console variable plus one 0x26ea0 toggle item — a data row of the
CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 cluster model.

The pin that is new to this member is the leading record: `[0x3244978] = "IsPraying"` and
`[0x3244980] = 0x52a990("IsPraying")`, i.e. the core/hashed_name_ctors NameHashCell shape with the
0x52a990 FNV-1a helper the native tree already models (core/fnv_cstr_161fb0). The toggle's gate is the
same float class as slot 379 (0x15942b0 -> slots {1, 3}), so its range narrows to {0.1, 0.5}.

Offline and read-only; the shared pinning work lives in tools/lift/debug_page_batch_lib.py. Skips
cleanly (exit 0) when the private inputs are absent. `--tamper elf|native` must exit 1.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import debug_page_batch_lib as lib  # noqa: E402

SPEC = {
    "index": 374,
    "rva": 0x701230,
    "size": 537,                       # FDE [0x701230, 0x701449)
    "sha": "3a60332f84349de345c778dd7b0b2ada646adc282e97c681f3b2699929ae7ce0",
    "table_word": 0x2DD6530,           # 0x2dd70e0 - 374*8
    "fde_addr": 0x144BE58,
    "fde_reclen": 36,
    "first_constant": "kSlot374Rva",
    "direct_calls": 11,
    "callees": 7,
    "rip_operands": 15,
    "rip_targets": 14,
    "header": "native/core/ctor_slot_374_701230.hpp",
    "source": "native/core/ctor_slot_374_701230.cpp",
    "test": "native/tests/test_ctor_slot_374.cpp",
    "constants": {
        "kSlot374Rva": 0x701230,
        "kSlot374BodySize": 537,
        "kSlot374EndRva": 0x701449,
        "kSlot374InstructionCount": 110,
        "kSlot374DirectCallSites": 11,
        "kSlot374DirectCallees": 7,
        "kSlot374FunctorSites": 1,
        "kSlot374BranchCount": 1,
        "kSlot374TableTopRva": 0x2DD70E0,
        "kSlot374TableWordRva": 0x2DD6530,
        "kSlot374TableIndex": 374,
        "kSlot374CanaryCellRva": 0x15FE518,
        "kSlot374CanaryFrameOffset": 0xC0,
        "kSlot374BranchRva": 0x701432,
        "kSlot374BranchTargetRva": 0x701443,
        "kSlot374RecordRva": 0x3244978,
        "kSlot374VarRva": 0x3244988,
        "kSlot374ItemRva": 0x3244A60,
        "kSlot374RecordNameRva": 0x1278C40,
        "kSlot374VarNameRva": 0x1278C4A,
        "kSlot374VarDescriptionRva": 0x1278CE5,
        "kSlot374ItemLabelRva": 0x1278C5A,
        "kSlot374ItemTextRva": 0x1278C66,
        "kSlot374RecordHash": 0x411FA39F,
        "kSlot374VarLoBits": 0x00000000,
        "kSlot374VarHiBits": 0x41200000,
        "kSlot374VarValueBits": 0x40000000,
        "kSlot374ToggleLoBits": 0x3DCCCCCD,
        "kSlot374ToggleHiBits": 0x3F000000,
    },
    "header_tokens": [
        '"IsPraying"',
        "{kSlot374RecordRva, kSlot374RecordName, 0x09, 0x701268, 0x70126b}",
        "debug_page::kArgFnVtblSlot374, 0x70133e, 0x70135a, 0x701376,",
        "debug_page::MenuKind::toggle_26ea0, kSlot374ItemRva, 0, kSlot374ItemLabel, 0x0b",
        "0.0f, 0.0f, 0x7013f9, 0x70141b}",
        "std::span<const debug_page::HashedNameRow>(kSlot374HashedNames)",
        "std::span<const debug_page::FloatVarRow>(kSlot374Vars)",
        "std::span<const debug_page::MenuRow>(kSlot374Items)",
    ],
    "source_order": [
        "debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1)",
        "std::span<DebugMenuItem>(&objects.item, 1)",
        "std::span<NameHashCell>(&objects.record, 1)",
        "return debug_page::init_debug_page(kSlot374Script, view, concmd, menu, hooks);",
    ],
    "test_tokens": [
        "section_constants", "section_native_build", "section_record_precedes_the_variable",
        "section_shape_refused", "section_register_refused", "section_item_register_refused",
        "section_canary",
        "r.hashed_names_built == 1",
        "fnv_cstr_52a990(kSlot374RecordName) == kSlot374RecordHash",
        "records[0].dtor_rva == 0x20f0",
        "records[1].dtor_rva == 0x272d0",
    ],
    "cmake_tokens": [
        "core/ctor_slot_374_701230.cpp",
        "add_test(NAME ctor_slot_374 COMMAND test_ctor_slot_374)",
    ],
    "calls": [
        (0x70126B, 0x52A990),   # the leading {name, fnv1a(name)} record
        (0x70133E, 0xC300),
        (0x70134A, 0xF590),     # f590(description)
        (0x701376, 0x20D60),
        (0x70137E, 0xF590),     # f590(name)
        (0x701397, 0xF8F390),   # __cxa_atexit(0x20f0, var)
        (0x7013F9, 0x26EA0),    # the toggle
        (0x701401, 0xF590),     # f590(text)
        (0x701409, 0xF590),     # f590(label)
        (0x70141B, 0xF8F390),   # __cxa_atexit(0x272d0, item)
        (0x701443, 0xF8F3A0),   # __stack_chk_fail (branch target only)
    ],
    "rips": [
        (0x701248, 7, 3, 0x15FE518),   # __stack_chk_guard GOT cell
        (0x70124F, 7, 3, 0x3244978),   # the {name, hash} record
        (0x701256, 7, 3, 0x1278C40),   # the record's name data
        (0x701273, 7, 3, 0x15942A0),   # 0x15942b0 - 0x10
        (0x70127A, 7, 3, 0x3244988),   # the variable object
        (0x701281, 7, 3, 0x1278C4A),   # name data
        (0x7012E6, 7, 3, 0x15CCCF0),   # caller argument functor tag
        (0x7012FE, 7, 3, 0x1278CE5),   # description data (empty)
        (0x701383, 7, 3, 0x1604218),   # __dso_handle
        (0x70138A, 7, 3, 0x20F0),      # the variable destructor
        (0x70139C, 7, 3, 0x3244A60),   # the toggle object
        (0x7013A3, 7, 3, 0x1278C5A),   # toggle label data
        (0x7013C6, 7, 3, 0x1278C66),   # toggle text data
        (0x70140E, 7, 3, 0x272D0),     # the item destructor
        (0x701420, 7, 3, 0x15FE518),   # the epilogue guard re-read
    ],
    "bytes": [
        (0x70123D, "4883e4e0", "and rsp,-0x20"),
        (0x701241, "4881ece0000000", "sub rsp,0xe0"),
        (0x701260, "48898424c0000000", "canary -> 0xc0(%rsp)"),
        (0x701268, "48893b", "mov [record], name (the leading {name, hash} cell)"),
        (0x701270, "894308", "mov [record+8], eax (0x52a990 result)"),
        (0x70135A, "ff5018", "callq *0x18(%rax)"),
        (0x701363, "41c786d000000000000040", "value store +0xd0 = 2.0f"),
        (0x70136E, "41c7466800000040", "default store +0x68 = 2.0f"),
        (0x70142A, "483b8424c0000000", "cmpq 0xc0(%rsp), %rax"),
        (0x701432, "750f", "jne 0x701443 (canary fail)"),
        (0x701434, "488d65d8", "leaq -0x28(%rbp), %rsp"),
        (0x701438, "5b415c415d415e415f5dc3", "pops + retq"),
        (0x701443, "e858df880090", "callq __stack_chk_fail + the body's last byte (nop)"),
    ],
    "branch": (0x701432, 0x701443),
    "functor_sites": [0x70135A],
    "functor_tags": [0x15CCCF0],
    "atexit": [
        (0x701397, 0x20F0, 0x3244988),
        (0x70141B, 0x272D0, 0x3244A60),
    ],
    "strings": [
        (0x1278C40, b"IsPraying"),
        (0x1278C4A, b"shadow_prayTime"),
        (0x1278CE5, b""),
        (0x1278C5A, b"Save System"),
        (0x1278C66, b"Shadow Prayer Time (s)"),
    ],
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, sys.argv[1:]))
