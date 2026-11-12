#!/usr/bin/env python3
"""ELF-tie checker for the family-admitting float constructor wrappers (CPU-01-HELPER-CALLEE-MODELS-01, 0x6aa00 / 0xc300).

Pins 0x6aa00 (0x142 B, unbounded) and 0xc300 (0x160 B, bounded) instruction by instruction, the float functor slot
bodies 0xc4a0/0xc4c0/0xc4e0/0xc4f0, the 68-table float functor family (recomputed from the relocation table, both
directions; the initial classes 0x1596878 / 0x1594318 must be members) and the call site of startup slot 504
(0x23f4d0), which passes tag 0x159ba68 to 0x6aa00 and runs the family's bare-ret destroy.

Usage: python3 -I tools/lift/check_helper_6aa00.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402

SPEC = {
    "name": "check_helper_6aa00",
    "summary": ("0x6aa00 (0x142 B, sha ee90dea5d0e2...) and 0xc300 (0x160 B) pinned per instruction, initial classes "
                "are float family tables, slot 504 passes float-family tag 0x159ba68 and runs the bare-ret destroy"),
    "families": ["float"],
    "family_count": {"float": 68},
    "bodies": [
        {"name": "0x6aa00 unbounded float ctor", "rva": 0x6AA00, "size": 0x142, "fde": 323, "pins": "ctor_6aa00",
         "sha": "ee90dea5d0e2ed59ce83ff52c5a3732419635622c7b422f3eff54fd1f5384ebb"},
        {"name": "0xc300 bounded float ctor", "rva": 0xC300, "size": 0x160, "fde": 352, "pins": "ctor_c300",
         "sha": "b0fad1ce8145048f5faf081eb6e4a03f97f9b8eb1cdaa44a76ff41e94f5f442f"},
        {"name": "float functor copy", "rva": 0xC4A0, "size": 0x14, "fde": 20, "pins": "ff_copy",
         "sha": "0137c6226a7dcb380747d73f16a87fab37c63e9c2b8f67d0d1a8f875f2d44a08"},
        {"name": "float functor clone", "rva": 0xC4C0, "size": 0x14, "fde": 20, "pins": "ff_clone",
         "sha": "5f93b13fd6577d64a7242762148d8d994a40ebd9480009297cce40f30caea597"},
        {"name": "float functor move", "rva": 0xC4E0, "size": 9, "fde": 9, "pins": "ff_move",
         "sha": "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"},
        {"name": "float functor destroy", "rva": 0xC4F0, "size": 1, "fde": 1, "pins": "ff_destroy",
         "sha": "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"},
    ],
    "rips": [
        ("0x6aa00 mid vtable table", 0x6AA31, 7, 3, 0x1594118),
        ("0x6aa00 initial class is a float family table", 0x6AA3C, 7, 3, 0x1596878, "float"),
        ("0xc300 mid vtable table", 0xC33B, 7, 3, 0x1594118),
        ("0xc300 initial class is a float family table", 0xC346, 7, 3, 0x1594318, "float"),
        ("float copy stores class 0x1594318", 0xC4A4, 7, 3, 0x1594318, "float"),
        ("float clone stores class 0x1594318", 0xC4C0, 7, 3, 0x1594318, "float"),
    ],
    "calls": [("0x6aa00 base ctor 0x20af0", 0x6AA2C, 0x20AF0), ("0x6aa00 stack check", 0x6AB3D, 0xF8F3A0),
              ("0xc300 base ctor 0x20af0", 0xC336, 0x20AF0), ("0xc300 stack check", 0xC45A, 0xF8F3A0)],
    "relocs": [("final float class slot 0 = dtor 0x20f0", 0x15942B0, 0x20F0)],
    "bytes": [("float functor destroy is a bare ret", 0xC4F0, "c3")],
    "sites": [
        {"name": "slot 504 (0x23f4d0)", "base": 0x23F4D0, "size": 562, "fde": 562, "family": "float",
         "sha": "46dc649502bece49df2b8bfbfce883785c00f3775a02475e6cf9b95faf7dffd4",
         "lea": 0x23F567, "tag": 0x159BA68, "call": 0x23F59E, "callee": 0x6AA00, "pins": "site_504"},
    ],
    "native": {
        "tags_h": "native/core/helper_functor_tags.hpp", "helper_h": "native/core/helper_6aa00_unbounded_float.hpp",
        "helper_c": "native/core/helper_6aa00_unbounded_float.cpp", "model_h": "native/core/ctor_shapes_343_356.hpp",
        "vars_h": "native/core/concmd_vars.hpp", "test": "native/tests/test_helper_6aa00.cpp",
        "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "tags_h": {"kFloatFunctorCopyRva": 0xC4A0, "kFloatFunctorCloneRva": 0xC4C0, "kFloatFunctorMoveRva": 0xC4E0,
                   "kFloatFunctorDestroyRva": 0xC4F0, "kFloatFunctorClassTag": 0x1594318,
                   "kFloatFunctorTagCount": 68},
        "model_h": {"kUnboundedFloatCtorRva": 0x6AA00, "kUnboundedFloatCtorSize": 0x142,
                    "kUnboundedFloatInitialFnVtbl": 0x1596878, "kUnboundedFloatMidVptr": 0x1594128},
        "vars_h": {"kRangedFloatCtorRva": 0xC300, "kRangedFloatCtorSize": 0x160, "kRangedFloatMidVptr": 0x1594128,
                   "kRangedFloatFinalVptr": 0x15942B0, "kRangedFloatFnVtbl": 0x1594318,
                   "kRangedFloatDtorRva": 0x20F0},
    },
    "tokens": {
        "helper_h": ["helper_6aa00_unbounded_float(", "helper_c300_bounded_float(", "init_float_var_static_family("],
        "helper_c": ["if (!is_float_functor_tag(args.fn_vptr)) {", "lifted.fn_vptr = kRangedFloatFnVtbl;",
                     "init_unbounded_float_var_6aa00(var, name, value_ref, flags, normalised(args), description)",
                     "init_bounded_float_var_c300(var, name, value_ref, min_value, max_value, flags, normalised(args),",
                     "out.functor_destroy_ok = ctor.arg_destroy_ok;",
                     "out.atexit_recorded = registry.atexit_log.record(kRangedFloatDtorRva, &var);",
                     "out.refusal = HelperRefusal::unadmitted_arg_functor;"],
        "test": ["CHECK(same_bytes(u, ref_u) && same_bytes(b, ref_b));", "helper_6aa00_unbounded_float(v, name, &cell, 1, make_args(0x159ba68, 0x42, 0x43), desc)",
                 "CHECK(!f.ok && !f.ctor_ok && !f.atexit_recorded && reg.atexit_log.size() == 0);",
                 "CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && all_poison(v));"],
        "cmake": ["core/helper_6aa00_unbounded_float.cpp", "add_test(NAME helper_6aa00 COMMAND test_helper_6aa00)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
