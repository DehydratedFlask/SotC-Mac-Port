#!/usr/bin/env python3
"""ELF-tie checker for the family-admitting unbounded-int constructor wrapper (CPU-01-HELPER-CALLEE-MODELS-01, 0x3a980).

Pins 0x3a980 (0x142 B) instruction by instruction, the int functor slot bodies, the 37-table int functor family
(recomputed from the relocation table, both directions; its seed class 0x1595660 must be a member) and the call site
of startup slot 555 (0x527f0), which passes tag 0x1595e60 to 0x3a980 and runs the family's bare-ret destroy.

Usage: python3 -I tools/lift/check_helper_3a980.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402

SPEC = {
    "name": "check_helper_3a980",
    "summary": ("0x3a980 (0x142 B, sha 2cbfc2763abb...) pinned per instruction, seed class 0x1595660 is an int family "
                "table, slot 555 passes int-family tag 0x1595e60 and runs the bare-ret destroy"),
    "families": ["int"],
    "family_count": {"int": 37},
    "bodies": [
        {"name": "0x3a980 unbounded int ctor", "rva": 0x3A980, "size": 0x142, "fde": 323, "pins": "ctor_3a980",
         "sha": "2cbfc2763abb6506a59551b902dd6098325889e5e520d842c0e93da7f43498ca"},
        {"name": "int functor copy", "rva": 0xC720, "size": 0x14, "fde": 20, "pins": "fi_copy",
         "sha": "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"},
        {"name": "int functor clone", "rva": 0xC740, "size": 0x14, "fde": 20, "pins": "fi_clone",
         "sha": "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"},
        {"name": "int functor move", "rva": 0xC760, "size": 9, "fde": 9, "pins": "fi_move",
         "sha": "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"},
        {"name": "int functor destroy", "rva": 0xC770, "size": 1, "fde": 1, "pins": "fi_destroy",
         "sha": "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"},
    ],
    "rips": [
        ("0x3a980 mid vtable table", 0x3A9B1, 7, 3, 0x1594190),
        ("0x3a980 seed class is an int family table", 0x3A9BC, 7, 3, 0x1595660, "int"),
        ("int copy stores class 0x15944a0", 0xC724, 7, 3, 0x15944A0, "int"),
        ("int clone stores class 0x15944a0", 0xC740, 7, 3, 0x15944A0, "int"),
    ],
    "calls": [("0x3a980 base ctor 0x20af0", 0x3A9AC, 0x20AF0), ("0x3a980 stack check", 0x3AABD, 0xF8F3A0)],
    "relocs": [("final int class slot 0 = dtor 0x18a60", 0x1594C08, 0x18A60)],
    "bytes": [("int functor destroy is a bare ret", 0xC770, "c3")],
    "sites": [
        {"name": "slot 555 (0x527f0)", "base": 0x527F0, "size": 585, "fde": 585, "family": "int",
         "sha": "190562fd7b18b100ac1e02949fecbc9c110d90e525165bd8e46ed2965a2e58d0",
         "lea": 0x528DB, "tag": 0x1595E60, "call": 0x5293D, "callee": 0x3A980, "pins": "site_555"},
    ],
    "native": {
        "tags_h": "native/core/helper_functor_tags.hpp", "helper_h": "native/core/helper_3a980_unbounded_int.hpp",
        "helper_c": "native/core/helper_3a980_unbounded_int.cpp", "model_h": "native/core/ctor_slots_322_334.hpp",
        "test": "native/tests/test_helper_3a980.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "tags_h": {"kIntFunctorClassTag": 0x15944A0, "kIntFunctorTagCount": 37},
        "model_h": {"kUnboundedIntWorkerRva": 0x3A980, "kUnboundedIntWorkerSize": 0x142,
                    "kUnboundedIntWorkerVptr": 0x15941A0, "kUnboundedIntFinalVptr": 0x1594C08,
                    "kUnboundedIntSeedFnTable": 0x1595660, "kUnboundedIntFnTable": 0x15944A0},
    },
    "tokens": {
        "helper_h": ["helper_3a980_unbounded_int(", "#include \"helper_c580_ranged_int.hpp\""],
        "helper_c": ["if (!is_int_functor_tag(block.fn_vptr)) {", "lifted.fn_vptr = kUnboundedIntFnTable;",
                     "init_unbounded_int_var_worker_3a980(var, name, flags, lifted, description)",
                     "out.arg_destroy_ok = true;", "out.refusal = HelperRefusal::unadmitted_arg_functor;"],
        "test": ["ConCmdRangedVar ref_norm = ref;", "CHECK(same_bytes(v_norm, ref_norm));",
                 "helper_3a980_unbounded_int(v, name, 1, make_args(0x1595e60, 0x55, 0x66), desc)",
                 "CHECK(v.min_engaged == 0 && v.max_engaged == 0);      // unbounded", "CHECK(all_poison(v));"],
        "cmake": ["core/helper_3a980_unbounded_int.cpp", "add_test(NAME helper_3a980 COMMAND test_helper_3a980)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
