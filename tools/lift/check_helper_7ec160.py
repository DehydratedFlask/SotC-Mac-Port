#!/usr/bin/env python3
"""ELF byte-oracle for the local ranged "enum" console-variable constructor 0x7ec160 (0x1b3 B) and its inner constructor 0x7ec320 (0x15f B), used by startup slot 336 (0x7ec4c0).

0x7ec320 is the ranged-int constructor 0xc580 with the class pair (mid 0x15ce4c0 + 0x10, functor 0x15ce538): the two 351 byte bodies are compared outside six rel32 operands and the operands are resolved on both bodies.
0x7ec160 pre-stores the final vptr (0x15ce558 + 0x10), passes this + 0xd0 as the value reference, re-wraps the caller functor through the class 0x15ce538, stores *init into [this+0xd0] and [this+0x68] and registers
through 0x20d60. The class tables (mid 0x15ce4d0, final 0x15ce568: dtor 0x7ea160, slot 3 = slot 6 = `mov eax,1; ret`, functor 0x15ce538 = 0x7ec100/0x7ec120/0x7ec140/0x7ec150) are pinned by relocation.

NOT established: what the enum value means at run time (INFERENCE only), and the wrapper's functor re-wrap is modelled by its net effect (the wrapper bytes are hash/operand pinned, not semantically re-executed).
Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402


from slot_pin_util import require  # noqa: E402

# 0x7ec320 is the ranged-int constructor 0xc580 with a different class pair: the 351 byte bodies are identical except for
# six rel32 operands. (offset, length, kind) of each; the operands are resolved on BOTH bodies and the bytes outside the
# windows are compared through recording reads, so a mutation of either body is rejected.
TWIN_WINDOWS = ((0x14, 4), (0x37, 4), (0x3E, 4), (0x49, 4), (0x13E, 4), (0x15B, 4))
TWIN_SIZE = 0x15F
TWIN_OPERANDS = (
    # (insn offset, kind, c580 target, 7ec320 target)
    (0x11, "rip", 0x15FE518, 0x15FE518),     # stack guard load
    (0x36, "call", 0x20AF0, 0x20AF0),        # concmd base ctor
    (0x3B, "rip", 0x1594190, 0x15CE4C0),     # mid vtable table (+0x10 added by the next instruction)
    (0x46, "rip", 0x15944A0, 0x15CE538),     # functor class table
    (0x13B, "rip", 0x15FE518, 0x15FE518),    # stack guard reload
    (0x15A, "call", 0xF8F3A0, 0xF8F3A0),     # __stack_chk_fail
)


def x_enum_twin(e, spec) -> None:
    """0x7ec320 == 0xc580 outside six rel32 operands; the operands pin the enum class pair."""
    a, b = 0xC580, 0x7EC320
    cursor = 0
    for off, length in TWIN_WINDOWS + ((TWIN_SIZE, 0),):
        if off > cursor:
            require(e.read(a + cursor, off - cursor) == e.read(b + cursor, off - cursor),
                    f"0x7ec320 equals 0xc580 over [+{cursor:#x}, +{off:#x})")
        cursor = off + length
    for insn, kind, want_a, want_b in TWIN_OPERANDS:
        if kind == "rip":
            got_a, got_b = e.rip(a + insn, 7, 3), e.rip(b + insn, 7, 3)
        else:
            got_a, got_b = e.call(a + insn), e.call(b + insn)
        require(got_a == want_a, f"0xc580 operand at +{insn:#x}")
        require(got_b == want_b, f"0x7ec320 operand at +{insn:#x}")
    e.bytes_at(b + 0x4D, "4883c010", "0x7ec320: mid vptr = table + 0x10")


def x_enum_tables(e, spec) -> None:
    """The private class tables of the enum variable and its functor (all RELATIVE relocation words)."""
    for index, target in enumerate((0x7EB4F0, 0x7EB530, 0x20D60, 0x7EB570, 0x7EB580, 0x223C0, 0x7EB7F0, 0x7EB800)):
        require(e.reloc_target(0x15CE4D0 + 8 * index) == target, f"enum mid vtable 0x15ce4d0 slot {index}")
    for index, target in ((0, 0x7EA160), (2, 0x20D60), (3, 0x7EB570), (6, 0x7EB7F0)):
        require(e.reloc_target(0x15CE568 + 8 * index) == target, f"enum final vtable 0x15ce568 slot {index}")
    for index, target in enumerate((0x7EC100, 0x7EC120, 0x7EC140, 0x7EC150)):
        require(e.reloc_target(0x15CE538 + 8 * index) == target, f"enum functor table 0x15ce538 slot {index}")
    # the functor bodies store the class constant 0x15ce538 exactly as the int bodies store 0x15944a0
    require(e.rip(0x7EC100 + 4, 7, 3) == 0x15CE538, "enum functor copy stores class 0x15ce538")
    require(e.rip(0x7EC120, 7, 3) == 0x15CE538, "enum functor clone stores class 0x15ce538")
    require(e.rip(0xC720 + 4, 7, 3) == 0x15944A0, "int functor copy stores class 0x15944a0")
    require(e.rip(0xC740, 7, 3) == 0x15944A0, "int functor clone stores class 0x15944a0")
    # outside the 4 byte class operand the enum bodies are the int bodies
    require(e.read(0xC720, 7) == e.read(0x7EC100, 7) and e.read(0xC72B, 9) == e.read(0x7EC10B, 9), "enum functor copy == int copy outside the class operand")
    require(e.read(0xC740, 3) == e.read(0x7EC120, 3) and e.read(0xC747, 13) == e.read(0x7EC127, 13), "enum functor clone == int clone outside the class operand")
    e.bytes_at(0x7EB570, "b801000000c3", "enum slot 3: mov eax,1; ret")
    e.bytes_at(0x7EB7F0, "b801000000c3", "enum slot 6: mov eax,1; ret")
    e.bytes_at(0x7EC150, "c3", "enum functor destroy is a bare ret")


def x_enum_imports(e, spec) -> None:
    bp._bp_check_imports(e, ((0xF8F3A0, 0x15FFFC0, "__stack_chk_fail"),))


bp.lib.EXTRAS["enum_twin"] = x_enum_twin
bp.lib.EXTRAS["enum_tables"] = x_enum_tables
bp.lib.EXTRAS["enum_imports"] = x_enum_imports

SPEC = {
    "slot": -1,
    "label": 'helper_7ec160',
    "no_table": True,
    "base": 0x7ec160,
    "size": 435,
    "body_sha256": "30b38c418e5682d6024b6b14aae540471882733b02d715752a867ba753f25307",
    "pad": "90909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
    ],
    "lengths": [
    ],
    "pins": [
        ("rip", 0x7ec17b, 7, 3, 0x15fe518),
        ("rip", 0x7ec182, 7, 3, 0x15ce558),
        ("imm", 0x7ec1b6, "4983c610"),
        ("rip", 0x7ec1ba, 7, 3, 0x15ce538),
        ("imm", 0x7ec1cf, "48c78558ffffff00000000"),
        ("imm", 0x7ec1e1, "48c78570ffffff00000000"),
        ("imm", 0x7ec1f8, "c4e37d19c001"),
        ("imm", 0x7ec1fe, "c4e3f916c101"),
        ("imm", 0x7ec231, "ff5108"),
        ("imm", 0x7ec242, "ff10"),
        ("imm", 0x7ec258, "ff10"),
        ("imm", 0x7ec264, "ff10"),
        ("imm", 0x7ec283, "ff5018"),
        ("call", 0x7ec2b7, 0x7ec320),
        ("imm", 0x7ec2ca, "ff5018"),
        ("call", 0x7ec2e7, 0x20d60),
        ("rip", 0x7ec2ec, 7, 3, 0x15fe518),
        ("call", 0x7ec30e, 0xf8f3a0),
    ],
    "m6_callees": {
        'enum_ctor_7ec320': (0x7ec320, 351, "6e6cdf4449bffdf6f0a359b769c7bc3422695238036d6f0a796b42a255a6158b"),
        'ranged_int_ctor_c580': (0xc580, 351, "a93bc0f34cfd49fbbcbc7f2186b2668de6e49f5f54012caafca7f17dfb396bce"),
        'enum_fn_copy_7ec100': (0x7ec100, 20, "3384f75bd1d7da69c808683b9e21cb36e4830f2f6af14ffc8687de3624226158"),
        'enum_fn_clone_7ec120': (0x7ec120, 20, "73f9bd945b25fc65685896a58a1c7929d55bf7ffe4be098c447ff6ad072834df"),
        'enum_fn_move_7ec140': (0x7ec140, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
        'enum_fn_destroy_7ec150': (0x7ec150, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
        'int_fn_copy_c720': (0xc720, 20, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
        'int_fn_clone_c740': (0xc740, 20, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
        'enum_slot3_7eb570': (0x7eb570, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
        'enum_slot6_7eb7f0': (0x7eb7f0, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
        'enum_dtor_7ea160': (0x7ea160, 54, "87257a146e4d85fd80e52d4873c58f5def6dd6416cef73b298b416d3ead192df"),
    },
    "m6_bytes": [
        (0x7ec1ac, "488db3d0000000", "wrapper: rsi = this + 0xd0 (the variable's own value cell)"),
        (0x7ec1c5, "4c8933", 'wrapper: [this] = final vptr (pre-store)'),
        (0x7ec2a9, "488d8528ffffff", 'wrapper: rax = &lifted args (stack copy)'),
        (0x7ec2b3, "ff7518", 'wrapper: push description (0x18(%rbp))'),
        (0x7ec2cd, "4c8933", 'wrapper: [this] = final vptr (after the inner ctor)'),
        (0x7ec2d3, "488b8df8feffff", 'wrapper: rcx = init pointer'),
        (0x7ec2da, "8b01", 'wrapper: eax = *init'),
        (0x7ec2dc, "8983d0000000", 'wrapper: [this+0xd0] = eax (value)'),
        (0x7ec2e2, "8b01", 'wrapper: eax = *init (second read)'),
        (0x7ec2e4, "894368", 'wrapper: [this+0x68] = eax (default)'),
    ],
    "m6_extras": ['enum_twin', 'enum_tables', 'enum_imports'],
    "native": {'header': 'native/core/helper_7ec160_enum_var.hpp', 'source': 'native/core/helper_7ec160_enum_var.cpp', 'test': 'native/tests/test_helper_7ec160.cpp', 'consts': {'kEnumVarWrapperRva': 8307040, 'kEnumVarWrapperSize': 435, 'kEnumVarCtorRva': 8307488, 'kEnumVarCtorSize': 351, 'kEnumVarMidVptr': 22865104, 'kEnumVarFinalVptr': 22865256, 'kEnumVarFnVtbl': 22865208, 'kEnumVarDtorRva': 8298848, 'kEnumVarSlot3Rva': 8303984, 'kEnumVarSlot6Rva': 8304624, 'kEnumVarSlot3Result': 1, 'kEnumVarSlot6Result': 1},
        'header_tokens': ['IDENTITY TAGS', 'EnumVarCtorResult helper_7ec160_enum_var(', 'DebugMenuTarget enum_var_menu_target(', 'HelperIntSiteResult helper_init_enum_var_site('], 'source_order': ['if (args.fn_vptr != kEnumVarFnVtbl) {', 'var.base.vptr = kEnumVarFinalVptr;', 'lifted.fn_vptr = kRangedIntFnVtbl;', 'init_ranged_int_var_c580(var, name, &var.value, min_value, max_value, flags, lifted, description)', 'var.base.vptr = kEnumVarMidVptr;', 'var.fn_vptr = kEnumVarFnVtbl;', 'out.arg_destroy_ok = true;', 'var.base.vptr = kEnumVarFinalVptr;\n    var.value = static_cast<std::uint32_t>(initial);', 'var.default_value = static_cast<std::uint32_t>(initial);', 'register_concmd_20d60(var.base, registry)', 'release_inert_view_f590(description)', 'release_inert_view_f590(name)', 'registry.atexit_log.record(kEnumVarDtorRva, &var)'], 'test_tokens': ['kEnumVarWrapperRva == 0x7ec160 && kEnumVarWrapperSize == 0x1b3', 'CHECK(!is_int_functor_tag(kEnumVarFnVtbl)', 'CHECK(v.value_ref == &v.value);', 'CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.arg_destroy_ok);', 'CHECK(creg.atexit_log.size() == 1 && creg.atexit_log.at(0).dtor_rva == 0x7ea160', 'CHECK(enum_var_menu_target(v).known && enum_var_menu_target(v).slot3 == 1 && enum_var_menu_target(v).slot6 == 1);']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'wrapper 0x7ec160 + inner ctor 0x7ec320 (twin of 0xc580, 6 rel32 windows) + enum class tables 0x15ce4d0/0x15ce568/0x15ce538'))
