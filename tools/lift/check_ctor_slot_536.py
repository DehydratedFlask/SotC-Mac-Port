#!/usr/bin/env python3
"""ELF tie for startup constructor index 536 (RVA 0x1342d0, 659 B): TWO bounded float console variables
and no menu item — a data row of the CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 cluster model.

This is the "repeat the variable block" member: two 0xc300 sites, two 0x20d60 registrations, two
__cxa_atexit(0x20f0) records, two `callq *0x18(%rax)` sites and no 0x272d0 record at all. Offline and
read-only; the shared pinning work lives in tools/lift/debug_page_batch_lib.py. Skips cleanly (exit 0)
when the private inputs are absent.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import debug_page_batch_lib as lib  # noqa: E402

SPEC = {
    "index": 536,
    "rva": 0x1342D0,
    "size": 659,                       # FDE [0x1342d0, 0x134563)
    "sha": "4a920842c45355391817842919656fe127531782e7046d4b7eb5e0c9cdd3ab6a",
    "table_word": 0x2DD6020,           # 0x2dd70e0 - 536*8
    "fde_addr": 0x134E540,
    "fde_reclen": 36,
    "first_constant": "kSlot536Rva",
    "direct_calls": 11,
    "callees": 5,
    "rip_operands": 24,
    "rip_targets": 14,
    "header": "native/core/ctor_slot_536_1342d0.hpp",
    "source": "native/core/ctor_slot_536_1342d0.cpp",
    "test": "native/tests/test_ctor_slot_536.cpp",
    "constants": {
        "kSlot536Rva": 0x1342D0,
        "kSlot536BodySize": 659,
        "kSlot536EndRva": 0x134563,
        "kSlot536InstructionCount": 124,
        "kSlot536DirectCallSites": 11,
        "kSlot536DirectCallees": 5,
        "kSlot536FunctorSites": 2,
        "kSlot536BranchCount": 1,
        "kSlot536TableTopRva": 0x2DD70E0,
        "kSlot536TableWordRva": 0x2DD6020,
        "kSlot536TableIndex": 536,
        "kSlot536CanaryCellRva": 0x15FE518,
        "kSlot536CanaryFrameOffset": 0xC0,
        "kSlot536BranchRva": 0x13454C,
        "kSlot536BranchTargetRva": 0x13455D,
        "kSlot536Var1Rva": 0x2EE1340,
        "kSlot536Var2Rva": 0x2EE1418,
        "kSlot536Var1NameRva": 0xFA7820,
        "kSlot536Var2NameRva": 0xFA7838,
        "kSlot536DescriptionRva": 0xFA7855,
        "kSlot536LoBits": 0x00000000,
        "kSlot536HiBits": 0x3F800000,
        "kSlot536Var1ValueBits": 0x3F000000,
        "kSlot536Var2ValueBits": 0x00000000,
    },
    "header_tokens": [
        '"r_probePreviewRoughness"',
        '"r_probePreviewMetalness"',
        "debug_page::kArgFnVtblSlot536, 0x1343c1, 0x1343e2, 0x134403, 0x134421}",
        "debug_page::kArgFnVtblSlot536, 0x1344da, 0x1344f6, 0x134517, 0x134535}",
        "std::span<const debug_page::FloatVarRow>(kSlot536Vars)",
        "std::span<const debug_page::MenuRow>()",
    ],
    "source_order": [
        "debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var1, 2), std::span<DebugMenuItem>(), std::span<NameHashCell>()};",
        "return debug_page::init_debug_page(kSlot536Script, view, concmd, menu, hooks);",
    ],
    "test_tokens": [
        "section_constants", "section_native_build", "section_unpinned_tag_in_second_row", "section_shape_refused",
        "section_register_refused", "section_canary",
        "kSlot536Vars.size() == 2 && kSlot536Script.items.empty()",
        "concmd.atexit_log.at(0).dtor_rva == 0x20f0",
        "concmd.atexit_log.at(1).dtor_rva == 0x20f0",
    ],
    "cmake_tokens": [
        "core/ctor_slot_536_1342d0.cpp",
        "add_test(NAME ctor_slot_536 COMMAND test_ctor_slot_536)",
    ],
    "calls": [
        (0x1343C1, 0xC300),    # var1
        (0x1343CD, 0xF590),    # f590(description) #1
        (0x134403, 0x20D60),   # register #1
        (0x13440B, 0xF590),    # f590(name) #1
        (0x134421, 0xF8F390),  # __cxa_atexit(0x20f0, var1)
        (0x1344DA, 0xC300),    # var2
        (0x1344E6, 0xF590),    # f590(description) #2
        (0x134517, 0x20D60),   # register #2
        (0x13451F, 0xF590),    # f590(name) #2
        (0x134535, 0xF8F390),  # __cxa_atexit(0x20f0, var2)
        (0x13455D, 0xF8F3A0),  # __stack_chk_fail (branch target only)
    ],
    "rips": [
        (0x1342E8, 7, 3, 0x15FE518),   # __stack_chk_guard GOT cell
        (0x1342EF, 7, 3, 0x15942A0),   # 0x15942b0 - 0x10
        (0x1342F6, 7, 3, 0xFA7820),    # var1 name data
        (0x134301, 7, 3, 0x2EE1340),   # var1 object
        (0x134311, 7, 3, 0x15981A0),   # caller argument functor tag
        (0x134318, 7, 3, 0xFA7855),    # the shared description data (length 0)
        (0x13438B, 7, 3, 0x2EE1340),   # the pre-stored class tag
        (0x1343A3, 7, 3, 0x2EE1410),   # value_ref = var1 + 0xd0
        (0x1343E8, 7, 3, 0x2EE1340),   # the post-functor class tag
        (0x1343EF, 10, 2, 0x2EE1410),  # movl $0x3f000000, ...   # var1 value 0.5f
        (0x1343F9, 10, 2, 0x2EE13A8),  # movl $0x3f000000, ...   # var1 default 0.5f
        (0x134410, 7, 3, 0x20F0),      # the variable destructor
        (0x134417, 7, 3, 0x1604218),   # __dso_handle
        (0x134426, 7, 3, 0xFA7838),    # var2 name data
        (0x13442D, 7, 3, 0x2EE1418),   # var2 object
        (0x134462, 7, 3, 0x2EE1418),   # the pre-stored class tag
        (0x134471, 7, 3, 0x2EE14E8),   # value_ref = var2 + 0xd0
        (0x13448D, 7, 3, 0x15981A0),   # the same caller tag at the second site
        (0x1344FC, 7, 3, 0x2EE1418),   # the post-functor class tag
        (0x134503, 10, 2, 0x2EE14E8),  # movl $0x0, ...   # var2 value 0.0f
        (0x13450D, 10, 2, 0x2EE1480),  # movl $0x0, ...   # var2 default 0.0f
        (0x134524, 7, 3, 0x20F0),      # the variable destructor (second record)
        (0x13452B, 7, 3, 0x1604218),   # __dso_handle
        (0x13453A, 7, 3, 0x15FE518),   # the epilogue guard re-read
    ],
    "bytes": [
        (0x1342DD, "4883e4e0", "and rsp,-0x20"),
        (0x1342E1, "4881ece0000000", "sub rsp,0xe0"),
        (0x13433C, "48898424c0000000", "canary -> 0xc0(%rsp)"),
        (0x1343E2, "ff5018", "callq *0x18(%rax) #1"),
        (0x1343EF, "c70517d0da020000003f", "var1 value store +0xd0 = 0.5f"),
        (0x1343F9, "c705a5cfda020000003f", "var1 default store +0x68 = 0.5f"),
        (0x1344F6, "ff5018", "callq *0x18(%rax) #2"),
        (0x134503, "c705dbcfda0200000000", "var2 value store +0xd0 = 0.0f"),
        (0x13450D, "c70569cfda0200000000", "var2 default store +0x68 = 0.0f"),
        (0x134544, "483b8424c0000000", "cmpq 0xc0(%rsp), %rax"),
        (0x13454C, "750f", "jne 0x13455d (canary fail)"),
        (0x13454E, "488d65d8", "leaq -0x28(%rbp), %rsp"),
        (0x134552, "5b415c415d415e415f5dc3", "pops + retq"),
        (0x13455D, "e83eaee50090", "callq __stack_chk_fail + the body's last byte (nop)"),
    ],
    "branch": (0x13454C, 0x13455D),
    "functor_sites": [0x1343E2, 0x1344F6],
    "functor_tags": [0x15981A0],
    "atexit": [
        (0x134421, 0x20F0, 0x2EE1340),
        (0x134535, 0x20F0, 0x2EE1418),
    ],
    "strings": [
        (0xFA7820, b"r_probePreviewRoughness"),
        (0xFA7838, b"r_probePreviewMetalness"),
        (0xFA7855, b""),
    ],
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, sys.argv[1:]))
