#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 0 (RVA 0x26db0, 239 bytes), lane L4.

Shared shapes of lane L4: the 0x26db0 menu toggle (primary body), the 0x6aa00 unbounded float ctor and the 0x7d3af0 zero-vector dtor.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and indirect call of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings, the functor vtables and the hash of every shared callee. A mutation
self-test flips the first/last byte of every pin and perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

SPEC = {
    "slot": 0,
    "label": "343_356_shapes",
    "no_table": True,
    "base": 0x26db0,
    "size": 239,
    "body_sha256": "903ee574f90468eb5c2ed01f6d299b010dbafd8ed3d90f18ed9a283754a62c6e",
    "pad": "90",
    "callees": ['unbounded_float_ctor', 'bounded_float_ctor', 'concmd_base_ctor', 'zero_vector_dtor', 'bool_slot3', 'bool_slot6', 'release_f590'],
    "bool_tags": [],
    "float_tags": [0x15ce318, 0x15ce260, 0x15ce1c0, 0x1596878],
    "strings": [
    ],
    "lengths": [
    ],
    "pins": [
        ("imm", 0x26dc3, "48c70300000000"),
        ("imm", 0x26dd2, "48c7431800000000"),
        ("imm", 0x26dda, "c6432000"),
        ("imm", 0x26e00, "c7434802000000"),
        ("call", 0x26e2f, 0xf180),
        ("imm", 0x26e3b, "c783980000000000803f"),
        ("imm", 0x26e45, "c7839c00000000002041"),
        ("imm", 0x26e5c, "ff5018"),
        ("imm", 0x26e6a, "ff5030"),
        ("imm", 0x26e72, "48b8cdcccc3d0000003f"),
        ("rip", 0x26e83, 7, 3, 0x2dfd4b8),  #
        ("jmp", 0x26e9a, 0x268b0),
        ("rip", 0x6aa11, 7, 3, 0x15fe518),  # stack canary cell
        ("call", 0x6aa2c, 0x20af0),
        ("rip", 0x6aa31, 7, 3, 0x1594118),  # class vtable
        ("rip", 0x6aa3c, 7, 3, 0x1596878),  #
        ("imm", 0x6aa4e, "c6436c00"),
        ("imm", 0x6aa52, "c6437000"),
        ("imm", 0x6aa56, "c6437400"),
        ("imm", 0x6aa5a, "c6437800"),
        ("imm", 0x6aa6d, "48c783b000000000000000"),
        ("imm", 0x6aa78, "48c783c800000000000000"),
        ("imm", 0x6aacf, "ff5008"),
        ("imm", 0x6aae0, "ff10"),
        ("imm", 0x6aaf6, "ff10"),
        ("imm", 0x6ab02, "ff10"),
        ("imm", 0x6ab21, "ff5018"),
        ("call", 0x6ab3d, 0xf8f3a0),
        ("jmp", 0x7d3af8, 0x170c0),
    ],
    "native": {'header': 'native/core/ctor_shapes_343_356.hpp', 'source': 'native/core/ctor_shapes_343_356.cpp', 'test': 'native/tests/test_ctor_shapes_343_356.cpp', 'shapes_h': 'native/core/ctor_shapes_343_356.hpp', 'shapes_c': 'native/core/ctor_shapes_343_356.cpp', 'cmake': 'native/CMakeLists.txt', 'consts': {'kUnboundedFloatCtorRva': 436736, 'kUnboundedFloatCtorSize': 322, 'kUnboundedFloatInitialFnVtbl': 22636664, 'kUnboundedFloatMidVptr': 22626600, 'kMenuToggleFromVarCtorRva': 159152, 'kMenuToggleFromVarCtorSize': 239, 'kZeroVectorDtorRva': 8207088, 'kZeroVectorDtorSize': 14, 'kL4MenuItemFlags': 48, 'kFloatArgFnVtbl_347': 22864664, 'kFloatArgFnVtbl_350': 22864480, 'kFloatArgFnVtbl_353B': 22864320}, 'header_tokens': ['kFlagScaleQword = 0x3f80000000000001ull', 'init_unbounded_float_var_6aa00', 'init_menu_toggle_from_var_26db0', 'init_zero_vector24', 'init_flag_scale_cell'], 'source_order': ['bool init_bounded_float_var_c300(', 'bool init_unbounded_float_var_6aa00(', 'FloatVarResult init_float_var_static(', 'bool init_menu_toggle_from_var_26db0(', 'bool init_zero_vector24(', 'void init_flag_scale_cell('], 'test_tokens': ['section_functors()', 'section_unbounded_vs_bounded()', 'section_cells()', 'section_all_nine()'], 'shapes_tokens': ['var.min_engaged = 0;', 'var.max_engaged = 0;', 'var.fn_vptr = kUnboundedFloatInitialFnVtbl;', 'item.range_min = 0.1f;', 'item.range_max = 0.5f;', 'item.kind = kDebugMenuItemKind;', 'cell.begin = 0;', 'std::atomic_ref<std::int32_t>(cell.atomic_word).store(0, std::memory_order_seq_cst);'], 'cmake_tokens': ['core/ctor_shapes_343_356.cpp', 'add_test(NAME ctor_shapes_343_356 COMMAND test_ctor_shapes_343_356)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Shared shapes of lane L4: the 0x26db0 menu toggle (primary body), the 0x6aa00 unbounded float ctor and the 0x7d3af0 zero-vector dtor.'))
