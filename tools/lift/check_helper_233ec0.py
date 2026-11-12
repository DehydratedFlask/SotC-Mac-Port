#!/usr/bin/env python3
"""ELF byte-oracle for the local unbounded-int console-variable constructor 0x233ec0 (0x142 B, FDE 323 B), the twin of 0x3a980, used by startup slot 506 (0x2340b0).

0x233ec0 is the unbounded-int constructor 0x3a980 with a different class pair (mid 0x159b590 + 0x10, private seed functor class 0x159b680): the two 322 byte bodies are compared outside five rel32 operands and the operands are resolved on both bodies.
The private functor class 0x159b680 (copy 0x234050, clone 0x234070, move 0x234090, destroy 0x2340a0) is the int family's four slot shapes with the class constant 0x159b680 instead of 0x15944a0, so after the install sequence the object keeps
{payload, class 0x159b680, aux} for its own tag (and {payload, 0x15944a0, aux} for an int-family tag). The class tables (mid 0x159b5a0, final 0x159b618: dtor 0x230d60, slot 3 = slot 6 = `mov eax,1; ret`) are pinned by relocation.

NOT established: what the variable means at run time (INFERENCE only); the install sequence is modelled by its net effect on the object (the temporaries are not observable).
Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402


from slot_pin_util import require  # noqa: E402

# 0x233ec0 is the unbounded-int constructor 0x3a980 with a different class pair: the 322 byte bodies are identical except for five rel32
# operands. The operands are resolved on BOTH bodies and the bytes outside the windows are compared through recording reads, so a
# mutation of either body is rejected.
TWIN_WINDOWS = ((0x14, 4), (0x2D, 4), (0x34, 4), (0x3F, 4), (0x13E, 4))
TWIN_SIZE = 0x142
TWIN_OPERANDS = (
    # (insn offset, kind, 3a980 target, 233ec0 target)
    (0x11, "rip", 0x15FE518, 0x15FE518),     # stack guard load
    (0x2C, "call", 0x20AF0, 0x20AF0),        # concmd base ctor
    (0x31, "rip", 0x1594190, 0x159B590),     # mid vtable table (+0x10 added later)
    (0x3C, "rip", 0x1595660, 0x159B680),     # seed functor class table
    (0x13D, "call", 0xF8F3A0, 0xF8F3A0),     # __stack_chk_fail
)


def x_twin_ctor(e, spec) -> None:
    """0x233ec0 == 0x3a980 outside five rel32 operands; the operands pin the class pair."""
    a, b = 0x3A980, 0x233EC0
    cursor = 0
    for off, length in TWIN_WINDOWS + ((TWIN_SIZE, 0),):
        if off > cursor:
            require(e.read(a + cursor, off - cursor) == e.read(b + cursor, off - cursor),
                    f"0x233ec0 equals 0x3a980 over [+{cursor:#x}, +{off:#x})")
        cursor = off + length
    for insn, kind, want_a, want_b in TWIN_OPERANDS:
        if kind == "rip":
            got_a, got_b = e.rip(a + insn, 7, 3), e.rip(b + insn, 7, 3)
        else:
            got_a, got_b = e.call(a + insn), e.call(b + insn)
        require(got_a == want_a, f"0x3a980 operand at +{insn:#x}")
        require(got_b == want_b, f"0x233ec0 operand at +{insn:#x}")


def x_twin_tables(e, spec) -> None:
    """The private class tables of the twin variable and its seed functor (all RELATIVE relocation words)."""
    for index, target in enumerate((0x233440, 0x233480, 0x20D60, 0x2334C0, 0x2334D0, 0x223C0, 0x233510, 0x233520)):
        require(e.reloc_target(0x159B5A0 + 8 * index) == target, f"twin mid vtable 0x159b5a0 slot {index}")
    for index, target in ((0, 0x230D60), (1, 0x234010), (2, 0x20D60), (3, 0x2334C0), (6, 0x233510)):
        require(e.reloc_target(0x159B618 + 8 * index) == target, f"twin final vtable 0x159b618 slot {index}")
    for index, target in enumerate((0x234050, 0x234070, 0x234090, 0x2340A0)):
        require(e.reloc_target(0x159B680 + 8 * index) == target, f"twin functor table 0x159b680 slot {index}")
    # the private functor bodies store the class constant 0x159b680 exactly as the int bodies store 0x15944a0
    require(e.rip(0x234050 + 4, 7, 3) == 0x159B680, "twin functor copy stores class 0x159b680")
    require(e.rip(0x234070, 7, 3) == 0x159B680, "twin functor clone stores class 0x159b680")
    require(e.rip(0xC720 + 4, 7, 3) == 0x15944A0, "int functor copy stores class 0x15944a0")
    require(e.rip(0xC740, 7, 3) == 0x15944A0, "int functor clone stores class 0x15944a0")
    # outside the 4 byte class operand the twin bodies are the int bodies; move == move; destroy is a bare ret
    require(e.read(0xC720, 7) == e.read(0x234050, 7) and e.read(0xC72B, 9) == e.read(0x23405B, 9), "twin functor copy == int copy outside the class operand")
    require(e.read(0xC740, 3) == e.read(0x234070, 3) and e.read(0xC747, 13) == e.read(0x234077, 13), "twin functor clone == int clone outside the class operand")
    require(e.read(0xC760, 9) == e.read(0x234090, 9), "twin functor move == int move")
    e.bytes_at(0x2340A0, "c3", "twin functor destroy is a bare ret")
    e.bytes_at(0x2334C0, "b801000000c3", "twin slot 3: mov eax,1; ret")
    e.bytes_at(0x233510, "b801000000c3", "twin slot 6: mov eax,1; ret")


def x_twin_imports(e, spec) -> None:
    bp._bp_check_imports(e, ((0xF8F3A0, 0x15FFFC0, "__stack_chk_fail"),))


bp.lib.EXTRAS["twin_ctor"] = x_twin_ctor
bp.lib.EXTRAS["twin_tables"] = x_twin_tables
bp.lib.EXTRAS["twin_imports"] = x_twin_imports

SPEC = {
    "slot": -1,
    "label": 'helper_233ec0',
    "no_table": True,
    "base": 0x233ec0,
    "size": 323,
    "body_sha256": "70622283107c939188ab575b416e57caa6d2a011e6d6569c722cf567aed73d78",
    "pad": "90909090909090909090909090",
    "callees": ['concmd_base_ctor'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x233ed1, 7, 3, 0x15fe518),
        ("call", 0x233eec, 0x20af0),
        ("rip", 0x233ef1, 7, 3, 0x159b590),
        ("rip", 0x233efc, 7, 3, 0x159b680),
        ("imm", 0x233f03, "4883c010"),
        ("imm", 0x233f0e, "c6436c00"),
        ("imm", 0x233f12, "c6437000"),
        ("imm", 0x233f16, "c6437400"),
        ("imm", 0x233f1a, "c6437800"),
        ("imm", 0x233f2d, "48c783b000000000000000"),
        ("imm", 0x233f38, "48c783c800000000000000"),
        ("imm", 0x233f6e, "4883e880"),
        ("imm", 0x233f8f, "ff5008"),
        ("imm", 0x233fa0, "ff10"),
        ("imm", 0x233fb6, "ff10"),
        ("imm", 0x233fc2, "ff10"),
        ("imm", 0x233fe1, "ff5018"),
        ("call", 0x233ffd, 0xf8f3a0),
    ],
    "m6_callees": {
        'unbounded_int_ctor_3a980': (0x3a980, 322, "2cbfc2763abb6506a59551b902dd6098325889e5e520d842c0e93da7f43498ca"),
        'twin_ctor_233ec0': (0x233ec0, 322, "8cc4bc88431dd5584596794b315d60cf7b7fb8fc409af98f1f201b3c0389e427"),
        'twin_fn_copy_234050': (0x234050, 20, "44dc6354c1fd08ec8c83fdb1a7975cacf3055c42958d4b907bb80eab7b1ade3c"),
        'twin_fn_clone_234070': (0x234070, 20, "59dfb39fe29b291ab5a856eba4f50b3110add8f269e30f8424a0f7be48526e60"),
        'twin_fn_move_234090': (0x234090, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
        'twin_fn_destroy_2340a0': (0x2340a0, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
        'int_fn_copy_c720': (0xc720, 20, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
        'int_fn_clone_c740': (0xc740, 20, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
        'int_fn_move_c760': (0xc760, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
        'twin_slot3_2334c0': (0x2334c0, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
        'twin_slot6_233510': (0x233510, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
        'twin_dtor_230d60': (0x230d60, 54, "cb1d411473cb1562581dea8ee0c88e70eb7ae80036f57595bef6df94f10b1af0"),
        'twin_mid_dtor_234010': (0x234010, 62, "5ba67b4255b6534a8bf5fd51cb209afdd88eedd839316dbe6d87bf477b18002b"),
    },
    "m6_bytes": [
        (0x233f03, "4883c010", 'twin: mid vptr = table + 0x10'),
    ],
    "m6_extras": ['twin_ctor', 'twin_tables', 'twin_imports'],
    "native": {'header': 'native/core/helper_233ec0_unbounded_int_twin.hpp', 'source': 'native/core/helper_233ec0_unbounded_int_twin.cpp', 'test': 'native/tests/test_helper_233ec0.cpp', 'consts': {'kUnboundedIntTwinRva': 2309824, 'kUnboundedIntTwinSize': 322, 'kUnboundedIntTwinMidVptr': 22656416, 'kUnboundedIntTwinSeedFnTable': 22656640, 'kUnboundedIntTwinFinalVptr': 22656536, 'kUnboundedIntTwinDtorRva': 2297184, 'kUnboundedIntTwinSlot3Rva': 2307264, 'kUnboundedIntTwinSlot6Rva': 2307344, 'kUnboundedIntTwinSlot3Result': 1, 'kUnboundedIntTwinSlot6Result': 1},
        'header_tokens': ['IDENTITY TAGS', 'HelperCtorResult helper_233ec0_unbounded_int_twin(', 'DebugMenuTarget unbounded_int_twin_menu_target(', 'HelperIntSiteResult helper_init_unbounded_int_twin_site('], 'source_order': ['helper_3a980_unbounded_int(var, name, flags, lifted, description)', 'var.base.vptr = kUnboundedIntTwinMidVptr;', 'if (private_class) var.fn_vptr = kUnboundedIntTwinSeedFnTable;', 'var.base.vptr = kUnboundedIntTwinFinalVptr;     // 0x23414c', 'helper_233ec0_unbounded_int_twin(var, name, spec.flags, args, description)', 'release_inert_view_f590(description)', 'var.base.vptr = kUnboundedIntTwinFinalVptr;                      // [obj] = 0x159b618', 'register_concmd_20d60(var.base, registry)', 'release_inert_view_f590(name)', 'registry.atexit_log.record(kUnboundedIntTwinDtorRva, &var)'], 'test_tokens': ['kUnboundedIntTwinRva == 0x233ec0 && kUnboundedIntTwinSize == 0x142', 'CHECK(!is_int_functor_tag(kUnboundedIntTwinSeedFnTable))', 'CHECK(v.fn_vptr == kUnboundedIntTwinSeedFnTable && v.fn_payload == 0x55 && v.fn_aux == 0x66);', 'CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.arg_destroy_ok);', 'CHECK(creg.atexit_log.size() == 1 && creg.atexit_log.at(0).dtor_rva == 0x230d60', 'CHECK(t.known && t.slot3 == 1 && t.slot6 == 1 && t.object == &v);']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'twin ctor 0x233ec0 (twin of 0x3a980, 5 rel32 windows) + private functor class 0x159b680 + class tables 0x159b5a0/0x159b618'))
