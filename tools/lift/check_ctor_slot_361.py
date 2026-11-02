#!/usr/bin/env python3
"""ELF tie for startup constructor index 361 (RVA 0x777390, 528 B): one bounded float console variable
plus one 0x27050 slider item — a data row of the CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 cluster model.

Offline and read-only; the shared pinning work lives in tools/lift/debug_page_batch_lib.py. Everything
here is a byte read of the pinned image plus a read of the native tree; nothing is executed. Skips
cleanly (exit 0) when the private inputs are absent. `--tamper elf|native` must exit 1; `--no-mutate`
skips the mutation self-test.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import debug_page_batch_lib as lib  # noqa: E402

SPEC = {
    "index": 361,
    "rva": 0x777390,
    "size": 528,                       # FDE [0x777390, 0x7775a0)
    "sha": "be7d0f2898b17365b8d242e8207d61b39dc03f52d501da961321ef0b92ab36b4",
    "table_word": 0x2DD6598,           # 0x2dd70e0 - 361*8
    "fde_addr": 0x1454C74,
    "fde_reclen": 32,
    "first_constant": "kSlot361Rva",
    "direct_calls": 10,
    "callees": 6,
    "rip_operands": 15,
    "rip_targets": 14,
    "header": "native/core/ctor_slot_361_777390.hpp",
    "source": "native/core/ctor_slot_361_777390.cpp",
    "test": "native/tests/test_ctor_slot_361.cpp",
    "constants": {
        "kSlot361Rva": 0x777390,
        "kSlot361BodySize": 528,
        "kSlot361EndRva": 0x7775A0,
        "kSlot361InstructionCount": 107,
        "kSlot361DirectCallSites": 10,
        "kSlot361DirectCallees": 6,
        "kSlot361FunctorSites": 1,
        "kSlot361BranchCount": 1,
        "kSlot361TableTopRva": 0x2DD70E0,
        "kSlot361TableWordRva": 0x2DD6598,
        "kSlot361TableIndex": 361,
        "kSlot361CanaryCellRva": 0x15FE518,
        "kSlot361CanaryFrameOffset": 0xC0,
        "kSlot361BranchRva": 0x777589,
        "kSlot361BranchTargetRva": 0x77759A,
        "kSlot361VarRva": 0x3254FB0,
        "kSlot361ItemRva": 0x3255088,
        "kSlot361VarNameRva": 0x1282568,
        "kSlot361VarDescriptionRva": 0x1282696,
        "kSlot361ItemLabelRva": 0x1282587,
        "kSlot361ItemTextRva": 0x1282591,
        "kSlot361VarLoBits": 0x00000000,
        "kSlot361VarHiBits": 0x41F00000,
        "kSlot361VarValueBits": 0x41600000,
        "kSlot361SliderLoBits": 0x3DCCCCCD,
        "kSlot361SliderHiBits": 0x3F800000,
    },
    "header_tokens": [
        '"shadow_snakeBossDiveLoopLength"',
        "debug_page::kArgFnVtblSlot361, 0x777485, 0x7774a1, 0x7774bd,",
        "debug_page::MenuKind::slider_27050, kSlot361ItemRva, 0, kSlot361ItemLabel, 0x09",
        "0.1f, 1.0f, 0x777550, 0x777572}",
        "std::span<const debug_page::FloatVarRow>(kSlot361Vars)",
        "std::span<const debug_page::MenuRow>(kSlot361Items)",
    ],
    "source_order": [
        "debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1)",
        "std::span<DebugMenuItem>(&objects.item, 1)",
        "return debug_page::init_debug_page(kSlot361Script, view, concmd, menu, hooks);",
    ],
    "test_tokens": [
        "section_constants", "section_native_build", "section_unpinned_tag", "section_shape_refused",
        "section_register_refused", "section_item_register_refused", "section_canary",
        "kSlot361Vars.size() == 1 && kSlot361Items.size() == 1",
        "records[0].dtor_rva == 0x20f0",
        "records[1].dtor_rva == 0x272d0",
    ],
    "cmake_tokens": [
        "core/ctor_slot_361_777390.cpp",
        "add_test(NAME ctor_slot_361 COMMAND test_ctor_slot_361)",
    ],
    "calls": [
        (0x777485, 0xC300),   # the bounded float variable
        (0x777491, 0xF590),   # f590(description)
        (0x7774BD, 0x20D60),  # register-once
        (0x7774C5, 0xF590),   # f590(name)
        (0x7774DE, 0xF8F390),  # __cxa_atexit(0x20f0, var)
        (0x777550, 0x27050),  # the slider
        (0x777558, 0xF590),   # f590(text)
        (0x777560, 0xF590),   # f590(label)
        (0x777572, 0xF8F390),  # __cxa_atexit(0x272d0, item)
        (0x77759A, 0xF8F3A0),  # __stack_chk_fail (branch target only)
    ],
    "rips": [
        (0x7773A8, 7, 3, 0x15FE518),   # movq 0xe87169(%rip), %rax    # __stack_chk_guard GOT cell
        (0x7773AF, 7, 3, 0x15942A0),   # leaq 0xe1ceea(%rip), %rbx    # 0x15942b0 - 0x10
        (0x7773B6, 7, 3, 0x3254FB0),   # leaq 0x2addbf3(%rip), %r14   # the variable object
        (0x7773BD, 7, 3, 0x1282568),   # leaq 0xb0b1a4(%rip), %rcx    # name data
        (0x7773D1, 7, 3, 0x15CDA18),   # leaq 0xe56640(%rip), %rdx    # caller argument functor tag
        (0x777419, 7, 3, 0x1282696),   # leaq 0xb0b276(%rip), %rcx    # description data (empty)
        (0x7774CA, 7, 3, 0x1604218),   # leaq 0xe8cd47(%rip), %rbx    # __dso_handle
        (0x7774D1, 7, 3, 0x20F0),      # leaq -0x7753e8(%rip), %rdi   # the variable destructor
        (0x7774E3, 7, 3, 0x1282587),   # leaq 0xb0b09d(%rip), %rax    # slider label data
        (0x7774EA, 7, 3, 0x1282591),   # leaq 0xb0b0a0(%rip), %rcx    # slider text data
        (0x7774F1, 7, 3, 0x3255088),   # leaq 0x2addb90(%rip), %r15   # the item object
        (0x7774FC, 8, 4, 0x1282564),   # vmovss 0xb0b060(%rip), %xmm1 # slider max 1.0f
        (0x77753C, 8, 4, 0x1282560),   # vmovss 0xb0b01c(%rip), %xmm0 # slider min 0.1f
        (0x777565, 7, 3, 0x272D0),     # leaq -0x75029c(%rip), %rdi   # the item destructor
        (0x777577, 7, 3, 0x15FE518),   # movq 0xe86f9a(%rip), %rax    # the epilogue guard re-read
    ],
    "bytes": [
        (0x77739D, "4883e4e0", "and rsp,-0x20"),
        (0x7773A1, "4881ece0000000", "sub rsp,0xe0"),
        (0x7773FD, "48898424c0000000", "canary -> 0xc0(%rsp)"),
        (0x7774A1, "ff5018", "callq *0x18(%rax)"),
        (0x7774AA, "41c786d000000000006041", "value store +0xd0 = 14.0f"),
        (0x7774B5, "41c7466800006041", "default store +0x68 = 14.0f"),
        (0x777581, "483b8424c0000000", "cmpq 0xc0(%rsp), %rax"),
        (0x777589, "750f", "jne 0x77759a (canary fail)"),
        (0x77758B, "488d65d8", "leaq -0x28(%rbp), %rsp"),
        (0x77758F, "5b415c415d415e415f5dc3", "pops + retq"),
        (0x77759A, "e8017e810090", "callq __stack_chk_fail + the body's last byte (nop)"),
    ],
    "branch": (0x777589, 0x77759A),
    "functor_sites": [0x7774A1],
    "functor_tags": [0x15CDA18],
    "atexit": [
        (0x7774DE, 0x20F0, 0x3254FB0),
        (0x777572, 0x272D0, 0x3255088),
    ],
    "strings": [
        (0x1282568, b"shadow_snakeBossDiveLoopLength"),
        (0x1282696, b""),
        (0x1282587, b"Shadow AI"),
        (0x1282591, b"Snake Boss Dive Loop Audio Length"),
    ],
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, sys.argv[1:]))
