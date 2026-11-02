#!/usr/bin/env python3
"""ELF tie for startup constructor index 467 (RVA 0x3ca110, 537 B): one bounded float console variable
plus one 0x27050 slider item — a data row of the CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 cluster model.

Offline and read-only; the shared pinning work lives in tools/lift/debug_page_batch_lib.py. Skips cleanly
(exit 0) when the private inputs are absent. `--tamper elf|native` must exit 1.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import debug_page_batch_lib as lib  # noqa: E402

SPEC = {
    "index": 467,
    "rva": 0x3CA110,
    "size": 537,                       # FDE [0x3ca110, 0x3ca329)
    "sha": "b367d2359e0c235c0362657be40c2c120d063c724216c64f25edcdf0e2a4a8e5",
    "table_word": 0x2DD6248,           # 0x2dd70e0 - 467*8
    "fde_addr": 0x13C76AC,
    "fde_reclen": 32,
    "first_constant": "kSlot467Rva",
    "direct_calls": 10,
    "callees": 6,
    "rip_operands": 20,
    "rip_targets": 16,
    "header": "native/core/ctor_slot_467_3ca110.hpp",
    "source": "native/core/ctor_slot_467_3ca110.cpp",
    "test": "native/tests/test_ctor_slot_467.cpp",
    "constants": {
        "kSlot467Rva": 0x3CA110,
        "kSlot467BodySize": 537,
        "kSlot467EndRva": 0x3CA329,
        "kSlot467InstructionCount": 107,
        "kSlot467DirectCallSites": 10,
        "kSlot467DirectCallees": 6,
        "kSlot467FunctorSites": 1,
        "kSlot467BranchCount": 1,
        "kSlot467TableTopRva": 0x2DD70E0,
        "kSlot467TableWordRva": 0x2DD6248,
        "kSlot467TableIndex": 467,
        "kSlot467CanaryCellRva": 0x15FE518,
        "kSlot467CanaryFrameOffset": 0xC0,
        "kSlot467BranchRva": 0x3CA312,
        "kSlot467BranchTargetRva": 0x3CA323,
        "kSlot467VarRva": 0x2EF6E08,
        "kSlot467ItemRva": 0x2EF6EE0,
        "kSlot467VarNameRva": 0xFE7BC0,
        "kSlot467VarDescriptionRva": 0xFE7D50,
        "kSlot467ItemLabelRva": 0xFE7BD1,
        "kSlot467ItemTextRva": 0xFE7BD7,
        "kSlot467VarLoBits": 0x3DCCCCCD,
        "kSlot467VarHiBits": 0x41200000,
        "kSlot467VarValueBits": 0x3F800000,
        "kSlot467SliderLoBits": 0x3DCCCCCD,
        "kSlot467SliderHiBits": 0x3F800000,
    },
    "header_tokens": [
        '"consoleTextScale"',
        "debug_page::kArgFnVtblSlot467, 0x3ca209, 0x3ca225, 0x3ca246,",
        "debug_page::MenuKind::slider_27050, kSlot467ItemRva, 0, kSlot467ItemLabel, 0x05",
        "0.1f, 1.0f, 0x3ca2d9, 0x3ca2fb}",
        "std::span<const debug_page::FloatVarRow>(kSlot467Vars)",
        "std::span<const debug_page::MenuRow>(kSlot467Items)",
    ],
    "source_order": [
        "debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1)",
        "std::span<DebugMenuItem>(&objects.item, 1)",
        "return debug_page::init_debug_page(kSlot467Script, view, concmd, menu, hooks);",
    ],
    "test_tokens": [
        "section_constants", "section_native_build", "section_unpinned_tag", "section_shape_refused",
        "section_register_refused", "section_item_register_refused", "section_canary",
        "kSlot467Vars.size() == 1 && kSlot467Items.size() == 1",
        "records[0].dtor_rva == 0x20f0",
        "records[1].dtor_rva == 0x272d0",
    ],
    "cmake_tokens": [
        "core/ctor_slot_467_3ca110.cpp",
        "add_test(NAME ctor_slot_467 COMMAND test_ctor_slot_467)",
    ],
    "calls": [
        (0x3CA209, 0xC300),
        (0x3CA215, 0xF590),    # f590(description)
        (0x3CA246, 0x20D60),
        (0x3CA24E, 0xF590),    # f590(name)
        (0x3CA267, 0xF8F390),  # __cxa_atexit(0x20f0, var)
        (0x3CA2D9, 0x27050),
        (0x3CA2E1, 0xF590),    # f590(text)
        (0x3CA2E9, 0xF590),    # f590(label)
        (0x3CA2FB, 0xF8F390),  # __cxa_atexit(0x272d0, item)
        (0x3CA323, 0xF8F3A0),  # __stack_chk_fail (branch target only)
    ],
    "rips": [
        (0x3CA128, 7, 3, 0x15FE518),   # movq 0x12343e9(%rip), %rax   # __stack_chk_guard GOT cell
        (0x3CA12F, 7, 3, 0x15942A0),   # leaq 0x11ca16a(%rip), %rbx   # 0x15942b0 - 0x10
        (0x3CA136, 7, 3, 0xFE7BC0),    # leaq 0xc1da83(%rip), %rcx    # name data
        (0x3CA141, 7, 3, 0x2EF6E08),   # leaq 0x2b2ccc0(%rip), %r15   # the variable object
        (0x3CA151, 7, 3, 0x15B0E90),   # leaq 0x11e6d38(%rip), %rdx   # caller argument functor tag
        (0x3CA199, 7, 3, 0xFE7D50),    # leaq 0xc1dbb0(%rip), %rcx    # description data (empty)
        (0x3CA1C0, 7, 3, 0x2EF6E08),   # movq %rbx, 0x2b2cc41(%rip)   # the pre-stored class tag
        (0x3CA1FA, 7, 3, 0x2EF6ED8),   # leaq 0x2b2ccd7(%rip), %rdx   # value_ref = var + 0xd0
        (0x3CA22B, 7, 3, 0x2EF6E08),   # movq %rbx, 0x2b2cbd6(%rip)   # the post-functor class tag
        (0x3CA232, 10, 2, 0x2EF6ED8),  # movl $0x3f800000, 0x2b2cc9c(%rip)   # value 1.0f
        (0x3CA23C, 10, 2, 0x2EF6E70),  # movl $0x3f800000, 0x2b2cc2a(%rip)   # default 1.0f
        (0x3CA253, 7, 3, 0x1604218),   # leaq 0x1239fbe(%rip), %r14   # __dso_handle
        (0x3CA25A, 7, 3, 0x20F0),      # leaq -0x3c8171(%rip), %rdi   # the variable destructor
        (0x3CA26C, 7, 3, 0xFE7BD1),    # leaq 0xc1d95e(%rip), %rax    # slider label data
        (0x3CA273, 7, 3, 0xFE7BD7),    # leaq 0xc1d95d(%rip), %rcx    # slider text data
        (0x3CA27E, 8, 4, 0xFE7B2C),    # vmovss 0xc1d8a6(%rip), %xmm1 # slider max 1.0f
        (0x3CA286, 7, 3, 0x2EF6EE0),   # leaq 0x2b2cc53(%rip), %r12   # the item object
        (0x3CA2C5, 8, 4, 0xFE7B28),    # vmovss 0xc1d85b(%rip), %xmm0 # slider min 0.1f
        (0x3CA2EE, 7, 3, 0x272D0),     # leaq -0x3a3025(%rip), %rdi   # the item destructor
        (0x3CA300, 7, 3, 0x15FE518),   # movq 0x1234211(%rip), %rax   # the epilogue guard re-read
    ],
    "bytes": [
        (0x3CA11D, "4883e4e0", "and rsp,-0x20"),
        (0x3CA121, "4881ece0000000", "sub rsp,0xe0"),
        (0x3CA17D, "48898424c0000000", "canary -> 0xc0(%rsp)"),
        (0x3CA225, "ff5018", "callq *0x18(%rax)"),
        (0x3CA232, "c7059cccb2020000803f", "value store +0xd0 = 1.0f (rip-relative)"),
        (0x3CA23C, "c7052accb2020000803f", "default store +0x68 = 1.0f (rip-relative)"),
        (0x3CA30A, "483b8424c0000000", "cmpq 0xc0(%rsp), %rax"),
        (0x3CA312, "750f", "jne 0x3ca323 (canary fail)"),
        (0x3CA314, "488d65d8", "leaq -0x28(%rbp), %rsp"),
        (0x3CA318, "5b415c415d415e415f5dc3", "pops + retq"),
        (0x3CA323, "e87850bc0090", "callq __stack_chk_fail + the body's last byte (nop)"),
    ],
    "branch": (0x3CA312, 0x3CA323),
    "functor_sites": [0x3CA225],
    "functor_tags": [0x15B0E90],
    "atexit": [
        (0x3CA267, 0x20F0, 0x2EF6E08),
        (0x3CA2FB, 0x272D0, 0x2EF6EE0),
    ],
    "strings": [
        (0xFE7BC0, b"consoleTextScale"),
        (0xFE7D50, b""),
        (0xFE7BD1, b"Debug"),
        (0xFE7BD7, b"Console Text Scale"),
    ],
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, sys.argv[1:]))
