#!/usr/bin/env python3
"""ELF-tie checker for the family-admitting ranged-int constructor wrapper (CPU-01-HELPER-CALLEE-MODELS-01, 0xc580).

Pins 0xc580 (0x15f B) instruction by instruction, the int functor slot bodies 0xc720/0xc740/0xc760/0xc770, the
37-table int functor family that `core/helper_functor_tags.hpp` admits (recomputed from the relocation table, both
directions), and the call site of startup slot 430 (0x529df0), which passes tag 0x15c8450 to 0xc580 and then runs the
family's bare-ret destroy. See tools/lift/helper_callee_lib.py for the layers and the mutation self-test.

Usage: python3 -I tools/lift/check_helper_c580.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import helper_callee_lib as lib  # noqa: E402

SPEC = {
    "name": "check_helper_c580",
    "summary": ("0xc580 (0x15f B, sha a93bc0f34cfd...) pinned per instruction, family clone/copy/move/destroy bodies "
                "0xc720..0xc770, slot 430 passes int-family tag 0x15c8450 and runs the bare-ret destroy"),
    "families": ["int"],
    "family_count": {"int": 37},
    "bodies": [
        {"name": "0xc580 ranged int ctor", "rva": 0xC580, "size": 0x15F, "fde": 352, "pins": "ctor_c580",
         "sha": "a93bc0f34cfd49fbbcbc7f2186b2668de6e49f5f54012caafca7f17dfb396bce"},
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
        ("0xc580 mid vtable table", 0xC5BB, 7, 3, 0x1594190),
        ("0xc580 initial class is an int family table", 0xC5C6, 7, 3, 0x15944A0, "int"),
        ("int copy stores class 0x15944a0", 0xC724, 7, 3, 0x15944A0, "int"),
        ("int clone stores class 0x15944a0", 0xC740, 7, 3, 0x15944A0, "int"),
    ],
    "calls": [("0xc580 base ctor 0x20af0", 0xC5B6, 0x20AF0), ("0xc580 stack check", 0xC6DA, 0xF8F3A0)],
    "relocs": [("final int class slot 0 = dtor 0x18a60", 0x1594C08, 0x18A60)],
    "bytes": [("int functor destroy is a bare ret", 0xC770, "c3")],
    "sites": [
        {"name": "slot 430 (0x529df0)", "base": 0x529DF0, "size": 539, "fde": 539, "family": "int",
         "sha": "ec8738216cc8bc929435eb74f01808fb04a13042fa7478d5a4f94f6686a3c93d",
         "lea": 0x529EC8, "tag": 0x15C8450, "call": 0x529F02, "callee": 0xC580, "pins": "site_430"},
    ],
    "native": {
        "tags_h": "native/core/helper_functor_tags.hpp", "tags_c": "native/core/helper_functor_tags.cpp",
        "helper_h": "native/core/helper_c580_ranged_int.hpp", "helper_c": "native/core/helper_c580_ranged_int.cpp",
        "model_h": "native/core/ranged_int_var_c580.hpp", "test": "native/tests/test_helper_c580.cpp",
        "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "tags_h": {"kIntFunctorCopyRva": 0xC720, "kIntFunctorCloneRva": 0xC740, "kIntFunctorMoveRva": 0xC760,
                   "kIntFunctorDestroyRva": 0xC770, "kIntFunctorClassTag": 0x15944A0, "kIntFunctorTagCount": 37},
        "model_h": {"kRangedIntCtorRva": 0xC580, "kRangedIntCtorSize": 0x15F, "kRangedIntMidVptr": 0x15941A0,
                    "kRangedIntFnVtbl": 0x15944A0, "kRangedIntFinalVptr": 0x1594C08, "kRangedIntDtorRva": 0x18A60},
    },
    "tokens": {
        "tags_c": ["std::binary_search(kIntFunctorTags.begin(), kIntFunctorTags.end(), tag)"],
        "helper_h": ["helper_c580_ranged_int(", "struct HelperCtorResult", "bool arg_destroy_ok = false;"],
        "helper_c": ["if (!is_int_functor_tag(args.fn_vptr)) {", "lifted.fn_vptr = kRangedIntFnVtbl;",
                     "init_ranged_int_var_c580(var, name, value_ref, min_value, max_value, flags, lifted, description)",
                     "out.arg_destroy_ok = true;", "out.refusal = HelperRefusal::unadmitted_arg_functor;"],
        "test": ["is_int_functor_tag(0x15c8450)", "CHECK(same_bytes(v, ref));", "CHECK(v.fn_vptr == kRangedIntFnVtbl);",
                 "CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.arg_destroy_ok);",
                 "CHECK(all_poison(v));"],
        "cmake": ["core/helper_c580_ranged_int.cpp", "core/helper_functor_tags.cpp",
                  "add_test(NAME helper_c580 COMMAND test_helper_c580)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
