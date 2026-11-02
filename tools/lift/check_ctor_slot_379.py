#!/usr/bin/env python3
"""ELF tie for startup constructor index 379 (RVA 0x6d8d20, 512 B): one bounded float console variable
plus one 0x26ea0 TOGGLE item — a data row of the CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 cluster model.

The interesting pin is the toggle's gate: the item targets the float class 0x15942b0, whose relocated
slots 3 / 6 are 0xbba0 (`mov eax,1; ret`) and 0xbbf0 (`mov eax,3; ret`), so `slot3 == 1 && slot6 == 3`
holds and the item range narrows to {0.1, 0.5}. Offline and read-only; the shared pinning work lives in
tools/lift/debug_page_batch_lib.py. Skips cleanly (exit 0) when the private inputs are absent.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import debug_page_batch_lib as lib  # noqa: E402

SPEC = {
    "index": 379,
    "rva": 0x6D8D20,
    "size": 512,                       # FDE [0x6d8d20, 0x6d8f20)
    "sha": "a86294fbffb8d24dd998671642bf8c2eb888e346126288abadb29dfbe8935aba",
    "table_word": 0x2DD6508,           # 0x2dd70e0 - 379*8
    "fde_addr": 0x14487A8,
    "fde_reclen": 36,
    "first_constant": "kSlot379Rva",
    "direct_calls": 10,
    "callees": 6,
    "rip_operands": 13,
    "rip_targets": 12,
    "header": "native/core/ctor_slot_379_6d8d20.hpp",
    "source": "native/core/ctor_slot_379_6d8d20.cpp",
    "test": "native/tests/test_ctor_slot_379.cpp",
    "constants": {
        "kSlot379Rva": 0x6D8D20,
        "kSlot379BodySize": 512,
        "kSlot379EndRva": 0x6D8F20,
        "kSlot379InstructionCount": 105,
        "kSlot379DirectCallSites": 10,
        "kSlot379DirectCallees": 6,
        "kSlot379FunctorSites": 1,
        "kSlot379BranchCount": 1,
        "kSlot379TableTopRva": 0x2DD70E0,
        "kSlot379TableWordRva": 0x2DD6508,
        "kSlot379TableIndex": 379,
        "kSlot379CanaryCellRva": 0x15FE518,
        "kSlot379CanaryFrameOffset": 0xC0,
        "kSlot379BranchRva": 0x6D8F09,
        "kSlot379BranchTargetRva": 0x6D8F1A,
        "kSlot379VarRva": 0x323BE40,
        "kSlot379ItemRva": 0x323BF18,
        "kSlot379VarNameRva": 0x12761B8,
        "kSlot379VarDescriptionRva": 0x1276298,
        "kSlot379ItemLabelRva": 0x12761E3,
        "kSlot379ItemTextRva": 0x12761ED,
        "kSlot379VarLoBits": 0x00000000,
        "kSlot379VarHiBits": 0x42C80000,
        "kSlot379VarValueBits": 0x41700000,
        "kSlot379ToggleLoBits": 0x3DCCCCCD,
        "kSlot379ToggleHiBits": 0x3F000000,
    },
    "header_tokens": [
        '"shadow_buddhaBossPlayerOutsideAttackRadius"',
        "debug_page::kArgFnVtblSlot379, 0x6d8e15, 0x6d8e31, 0x6d8e4d,",
        "debug_page::MenuKind::toggle_26ea0, kSlot379ItemRva, 0, kSlot379ItemLabel, 0x09",
        "0.0f, 0.0f, 0x6d8ed0, 0x6d8ef2}",
        "std::span<const debug_page::FloatVarRow>(kSlot379Vars)",
        "std::span<const debug_page::MenuRow>(kSlot379Items)",
    ],
    "source_order": [
        "debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1)",
        "std::span<DebugMenuItem>(&objects.item, 1)",
        "return debug_page::init_debug_page(kSlot379Script, view, concmd, menu, hooks);",
    ],
    "test_tokens": [
        "section_constants", "section_native_build", "section_unpinned_tag", "section_kind_dispatch",
        "section_shape_refused", "section_register_refused", "section_item_register_refused", "section_canary",
        "lo_bits == 0x3dcccccd && hi_bits == 0x3f000000",
        "records[0].dtor_rva == 0x20f0",
        "records[1].dtor_rva == 0x272d0",
    ],
    "cmake_tokens": [
        "core/ctor_slot_379_6d8d20.cpp",
        "add_test(NAME ctor_slot_379 COMMAND test_ctor_slot_379)",
    ],
    "calls": [
        (0x6D8E15, 0xC300),
        (0x6D8E21, 0xF590),    # f590(description)
        (0x6D8E4D, 0x20D60),
        (0x6D8E55, 0xF590),    # f590(name)
        (0x6D8E6E, 0xF8F390),  # __cxa_atexit(0x20f0, var)
        (0x6D8ED0, 0x26EA0),   # the toggle
        (0x6D8ED8, 0xF590),    # f590(text)
        (0x6D8EE0, 0xF590),    # f590(label)
        (0x6D8EF2, 0xF8F390),  # __cxa_atexit(0x272d0, item)
        (0x6D8F1A, 0xF8F3A0),  # __stack_chk_fail (branch target only)
    ],
    "rips": [
        (0x6D8D38, 7, 3, 0x15FE518),   # __stack_chk_guard GOT cell
        (0x6D8D3F, 7, 3, 0x15942A0),   # 0x15942b0 - 0x10
        (0x6D8D46, 7, 3, 0x323BE40),   # the variable object
        (0x6D8D4D, 7, 3, 0x12761B8),   # name data
        (0x6D8D61, 7, 3, 0x15CC898),   # caller argument functor tag
        (0x6D8DA9, 7, 3, 0x1276298),   # description data (empty)
        (0x6D8E5A, 7, 3, 0x1604218),   # __dso_handle
        (0x6D8E61, 7, 3, 0x20F0),      # the variable destructor
        (0x6D8E73, 7, 3, 0x323BF18),   # the item object
        (0x6D8E7A, 7, 3, 0x12761E3),   # toggle label data
        (0x6D8E9D, 7, 3, 0x12761ED),   # toggle text data
        (0x6D8EE5, 7, 3, 0x272D0),     # the item destructor
        (0x6D8EF7, 7, 3, 0x15FE518),   # the epilogue guard re-read
    ],
    "bytes": [
        (0x6D8D2D, "4883e4e0", "and rsp,-0x20"),
        (0x6D8D31, "4881ece0000000", "sub rsp,0xe0"),
        (0x6D8D8D, "48898424c0000000", "canary -> 0xc0(%rsp)"),
        (0x6D8E31, "ff5018", "callq *0x18(%rax)"),
        (0x6D8E3A, "41c786d000000000007041", "value store +0xd0 = 15.0f"),
        (0x6D8E45, "41c7466800007041", "default store +0x68 = 15.0f"),
        (0x6D8F01, "483b8424c0000000", "cmpq 0xc0(%rsp), %rax"),
        (0x6D8F09, "750f", "jne 0x6d8f1a (canary fail)"),
        (0x6D8F0B, "488d65d8", "leaq -0x28(%rbp), %rsp"),
        (0x6D8F0F, "5b415c415d415e415f5dc3", "pops + retq"),
        (0x6D8F1A, "e881648b0090", "callq __stack_chk_fail + the body's last byte (nop)"),
    ],
    "branch": (0x6D8F09, 0x6D8F1A),
    "functor_sites": [0x6D8E31],
    "functor_tags": [0x15CC898],
    "atexit": [
        (0x6D8E6E, 0x20F0, 0x323BE40),
        (0x6D8EF2, 0x272D0, 0x323BF18),
    ],
    "strings": [
        (0x12761B8, b"shadow_buddhaBossPlayerOutsideAttackRadius"),
        (0x1276298, b""),
        (0x12761E3, b"Shadow AI"),
        (0x12761ED, b"Buddha Boss Player Outside Attack Radius"),
    ],
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, sys.argv[1:]))
