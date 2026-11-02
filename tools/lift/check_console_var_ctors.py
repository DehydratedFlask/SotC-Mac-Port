#!/usr/bin/env python3
"""ELF tie for the shared console-variable constructor family 0xa4f4d0 / 0xa4fec0 / 0xa50280.

Offline and read-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3)
are read through `slot_pin_util.Elf`; nothing is loaded, relocated or executed. Skips cleanly (exit 0)
when the private inputs are absent.

What is pinned here (everything is a byte read of the pinned image):

  * the six bodies and their FDE extents. Each wrapper is 0x1b4 (436) bytes and each leaf 0x160 (352)
    bytes, and both numbers come from .eh_frame_hdr (PT_GNU_EH_FRAME datarel sdata4 table), not from a
    disassembler; the whole-body sha256 of each of the six is pinned;
  * the start of every leaf is exactly the start of its class's non-deleting destructor minus 0x160,
    i.e. `leaf + kRangedBoundLeafSize == non_deleting_dtor`;
  * THE SHARED TEMPLATE: the three wrappers are byte-identical 436-byte bodies except for six 4-byte
    immediates (two `lea` identity operands and four address-dependent encodings of the same fixed
    targets), and the three leaves are byte-identical 352-byte bodies except for six. Even the
    wrapper's `call` to its own leaf is byte-identical, because every leaf sits exactly +0x1c0 after
    its wrapper. The checker masks exactly those fields, requires byte equality, then requires each
    masked field to resolve to the pinned map. That is a stronger statement than three separate
    hashes: it is the template identity;
  * the indirect-call and branch structure: 6 `callq *...` and 2 conditional branches per wrapper,
    5 and 2 per leaf, at pinned offsets with pinned encodings; the only branch that leaves the body
    is the `jne` into `__stack_chk_fail`, and the `je` is the `args.q[3] != 0` self-pointer test;
  * the class identities: mid address points 0x15d1a20 / 0x15d1a98 / 0x15d1b10 and final ones
    0x15d1ba8 / 0x15d1c60 / 0x15d1cf8, reached from the pinned `lea` operands; final slots
    0 = destructor, 1 = the non-deleting destructor right after the leaf, 2 = `0x20d60`,
    3 / 6 = `b8 01 00 00 00 c3`; functor classes 0x15d1b78 / 0x15d1c30 / 0x15d1cc8 with
    copy / clone / move-assign / bare-`ret`-destroy, and the copy storing the class tag at +0x20;
  * the caller side: the startup-table word 0x2dd70e0 - 309*8 holds 0xa505e0, and slot 309's three
    calls land on 0xa4f4d0 / 0xa4fec0 / 0xa50280 (the call sites are inside the 40 rows that
    tools/lift/check_ctor_slot_309.py pins);
  * the native side: the header constants of core/ranged_bound_var_a4f4d0.hpp, the store order of its
    implementation, the slot-309 hook wiring (default_slot309_hooks, the production row
    `{0xa505e0, 309, ...}`), the two fixtures and the CMake targets.

A mutation self-test flips the first and last byte of every recorded pin and requires every mutant to
be rejected (`--no-mutate` skips it). `--tamper elf|native` runs the check on a deliberately tampered
input and therefore exits 1: a checker that cannot fail proves nothing.

Not machine-checked here, and labelled as an observation: the "exactly one caller per body" census.
tools/lift/check_ctor_slot_309.py pins all 40 direct call sites of slot 309 (including the three
wrapper calls); the reverse direction was established with a full-image `callq` census and is
re-verified by `grep -c` over local/validation/registry-analysis-agent/all.dis (1 each).
"""

from __future__ import annotations

import re
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf, ROOT, const_value, digest, require  # noqa: E402

EH_FRAME_HDR = 0x150C08C
TABLE_TOP = 0x2DD70E0
SLOT309_RVA = 0xA505E0
SLOT309_INDEX = 309

WRAPPER_SIZE = 0x1B4
LEAF_SIZE = 0x160
GUARD_CELL = 0x15FE518
STACK_CHK_FAIL = 0xF8F3A0
BASE_CTOR = 0x20AF0
REGISTER = 0x20D60

# wrapper, leaf, mid vptr, functor class, final vptr, functor destroy, dtor, non-deleting dtor
CLASSES = [
    (0xA4F4D0, 0xA4F690, 0x15D1A20, 0x15D1B78, 0x15D1BA8, 0xA4F4C0, 0xA47D20, 0xA4F7F0),
    (0xA4FEC0, 0xA50080, 0x15D1A98, 0x15D1C30, 0x15D1C60, 0xA4FEB0, 0xA48240, 0xA501E0),
    (0xA50280, 0xA50440, 0x15D1B10, 0x15D1CC8, 0x15D1CF8, 0xA50270, 0xA4AB50, 0xA505A0),
]

# whole-body sha256 (FDE extent)
WRAPPER_SHA = [
    "c2316b0b2cbe34543afb98526314a08f54fd8f960cfea92f58826d92954df1bf",
    "6fbb332eeaebdfbad8172bf555967b57db55e8121df555e47b8740f0ca524d45",
    "934d3a23870767d4784c51d21865cc71f5200b5ad9acbcd9d7f6c899f9f816b2",
]
LEAF_SHA = [
    "f1cfb207325acfe2f52d4391d8a3ee21c0f4be95eb954243df871f173e9831a6",
    "74af47aaaf7917bf9fc9cf6e260d864fe639f9b61eecc58f26b81b2531d9efac",
    "209546e3d4e9704a52a2ad1d860ad44c1f1df66f4d0907d4468bbe4bd8778925",
]
DTOR_SHA = [
    "08eaabe9071feb0543b71bacb999a8965e5429e8e71fc2d61b10ca3a84cb10ee",
    "c152b9cc8c0085c0a051b14a003522e22f52cc0d529b3c7804b2321ef713fda3",
    "e95d748544e53cdebae322caabe0a15d8c4e92195f5794e5f235387dc7a26681",
]
NON_DELETING_SHA = [
    "d60d0535bae20ad74034d535f816fd1679ca9399a0b6b5a1029110676e8febf4",
    "e19120ee2fd1b53a083ccc89ae4dd4f3afec042e603ffe5b0dfe73dde05a4776",
    "425aee65d5f974b3159924cf8160478118ca9fff1fcafbf9aa9285ddc902f32d",
]
DTOR_SIZE = 54
NON_DELETING_DTOR_SIZE = 62

# .eh_frame_hdr FDE record length (the 4-byte `length` field), pinned per body class.
FDE_RECLEN_BODY = 32
FDE_RECLEN_DTOR = 24

# The only bytes that differ between two instantiations: six 4-byte immediates in each body. Two per
# body carry the class identity (`lea` operands); the other four are fixed targets encoded at a
# different instruction address (the __stack_chk_guard GOT cell twice, the 0x20d60 / 0x20af0 call, and
# the __stack_chk_fail call). The wrapper-to-leaf `call` is NOT in this list: every leaf sits exactly
# +0x1c0 after its wrapper, so its rel32 is byte-identical in all three.
#   wrapper: guard @+0x1b | lea final base @+0x22 | lea functor class @+0x5a | call 0x20d60 @+0x187
#            | epilogue guard @+0x18c | call __stack_chk_fail @+0x1ae
#   leaf:    guard @+0x11 | call 0x20af0 @+0x36 | lea mid base @+0x3b | lea functor class @+0x46
#            | epilogue guard @+0x13b | call __stack_chk_fail @+0x15a
WRAPPER_RELOC = [(0x1B + 3, 4, "guard"), (0x22 + 3, 4, "final-base"), (0x5A + 3, 4, "functor-class"),
                 (0x187 + 1, 4, "register-call"), (0x18C + 3, 4, "guard-epilogue"),
                 (0x1AE + 1, 4, "fail-call")]
LEAF_RELOC = [(0x11 + 3, 4, "guard"), (0x36 + 1, 4, "base-ctor-call"), (0x3B + 3, 4, "mid-base"),
              (0x46 + 3, 4, "functor-class"), (0x13B + 3, 4, "guard-epilogue"),
              (0x15A + 1, 4, "fail-call")]
WRAPPER_FIELDS = [(off, size) for off, size, _ in WRAPPER_RELOC]
LEAF_FIELDS = [(off, size) for off, size, _ in LEAF_RELOC]

# Prologues (push rbp / mov rsp,rbp / push r15..rbx / sub rsp,N).
WRAPPER_PROLOGUE = "554889e54157415641554154534881ece8000000"
LEAF_PROLOGUE = "554889e54157415641554154534883ec68"

# Indirect calls, by offset inside the body, in body order.
WRAPPER_INDIRECT = [
    (0xD1, "ff5108", "callq *0x8(%rcx)   # clone, through the CALLER's functor class"),
    (0xE2, "ff10", "callq *(%rax)      # copy tmp2 <- tmp1"),
    (0xF8, "ff10", "callq *(%rax)      # copy local.fn <- tmp1"),
    (0x104, "ff10", "callq *(%rax)      # copy local.fn <- tmp2"),
    (0x123, "ff5018", "callq *0x18(%rax)  # bare-ret destroy of tmp1"),
    (0x16A, "ff5018", "callq *0x18(%rax)  # bare-ret destroy of the wrapper's local block"),
]
LEAF_INDIRECT = [
    (0xE6, "ff5008", "callq *0x8(%rax)   # clone"),
    (0xF7, "ff10", "callq *(%rax)      # copy tmp2 <- tmp1"),
    (0x10D, "ff10", "callq *(%rax)      # copy tmp1 <- this.fn"),
    (0x119, "ff10", "callq *(%rax)      # copy this.fn <- tmp2"),
    (0x138, "ff5018", "callq *0x18(%rax)  # bare-ret destroy of tmp1"),
]

# Conditional branches: (offset, encoding, what). The `jne` is always the canary compare.
WRAPPER_BRANCHES = [(0xA7, "740e", "je +0x10  # args.q[3] == 0 skips the self-pointer fixup"),
                    (0x19A, "7512", "jne +0x12 # __stack_chk_guard changed -> __stack_chk_fail")]
LEAF_BRANCHES = [(0xC0, "740a", "je +0x0a  # args.q[3] == 0 skips the self-pointer fixup"),
                 (0x149, "750f", "jne +0x0f # __stack_chk_guard changed -> __stack_chk_fail")]

# The four functor slot bodies of each class: (copy, clone, move-assign, destroy).
FUNCTOR_SLOTS = [
    (0xA4F470, 0xA4F490, 0xA4F4B0, 0xA4F4C0),
    (0xA4FE60, 0xA4FE80, 0xA4FEA0, 0xA4FEB0),
    (0xA50220, 0xA50240, 0xA50260, 0xA50270),
]

NATIVE = {
    "header": "native/core/ranged_bound_var_a4f4d0.hpp",
    "source": "native/core/ranged_bound_var_a4f4d0.cpp",
    "test": "native/tests/test_ranged_bound_var_a4f4d0.cpp",
    "slot_h": "native/core/ctor_slot_309_a505e0.hpp",
    "slot_c": "native/core/ctor_slot_309_a505e0.cpp",
    "slot_test": "native/tests/test_ctor_slot_309.cpp",
    "startup_h": "native/core/startup_ctors.hpp",
    "startup_c": "native/core/startup_ctors.cpp",
    "coverage": "native/tests/test_startup_ctor_coverage.cpp",
    "cmake": "native/CMakeLists.txt",
}

CONSTANTS = {
    "kRangedBoundVarClassCount": 3,
    "kRangedBoundVarFlags": 1,
    "kRangedBoundWrapperSize": WRAPPER_SIZE,
    "kRangedBoundLeafSize": LEAF_SIZE,
    "kRangedBoundWrapperInstructions": 99,
    "kRangedBoundLeafInstructions": 90,
    "kRangedBoundWrapperIndirectCalls": 6,
    "kRangedBoundLeafIndirectCalls": 5,
    "kRangedBoundFunctorSlotCalls": 11,
    "kRangedBoundRegisterRva": REGISTER,
    "kRangedBoundBaseCtorRva": BASE_CTOR,
    "kRangedBoundStackChkFailRva": STACK_CHK_FAIL,
}

# The header's class table, in row order, as written in the source.
HEADER_ROWS = [
    "{0xa4f4d0, 0xa4f690, 0x15d1a20, 0x15d1b78, 0x15d1ba8, 0xa4f4c0, 0xa47d20, 0xa4f7f0}",
    "{0xa4fec0, 0xa50080, 0x15d1a98, 0x15d1c30, 0x15d1c60, 0xa4feb0, 0xa48240, 0xa501e0}",
    "{0xa50280, 0xa50440, 0x15d1b10, 0x15d1cc8, 0x15d1cf8, 0xa50270, 0xa4ab50, 0xa505a0}",
]

# The implementation's store sequence, in body order (each token must appear exactly once, in order).
SOURCE_ORDER = [
    "if (args.fn_vptr != cls.fn_vtbl) return out;",
    "if (!is_pinned_class(cls)) return out;",
    "if (value_ref == nullptr) return out;",
    "init_concmd_base_20af0(var.base, name, description, flags);",
    "var.base.vptr = cls.mid_vptr;",
    "var.value_ref = value_ref;",
    "var.min_value[0] = 0;",
    "var.min_engaged = 1;",
    "std::memcpy(var.min_value, &min_cell, 4);",
    "var.max_value[0] = 0;",
    "var.max_engaged = 1;",
    "std::memcpy(var.max_value, &max_cell, 4);",
    "std::memset(var.args, 0, sizeof var.args);",
    "var.fn_vptr = cls.fn_vtbl;",
    "var.fn_payload = 0;",
    "var.fn_aux = 0;",
    "install_ranged_fn_from_args(var, args, cls.fn_vtbl);",
    "var.base.vptr = cls.final_vptr;",
    "var.value = cell;",
    "var.default_value = cell;",
    "out.registration = register_concmd_20d60(var.base, registry);",
]

# Slot 309's spec table rows (ctor, leaf, final vptr, mid vptr, functor tag, dtor) as written.
SPEC_ROWS = [
    "{kSlot309VarCtor1Rva, 0xa4f690, kSlot309Var1Vptr, 0x15d1a20, kSlot309Var1FunctorTag, 0xa47d20,",
    "{kSlot309VarCtor2Rva, 0xa50080, kSlot309Var2Vptr, 0x15d1a98, kSlot309Var2FunctorTag, 0xa48240,",
    "{kSlot309VarCtor3Rva, 0xa50440, kSlot309Var3Vptr, 0x15d1b10, kSlot309Var3FunctorTag, 0xa4ab50,",
]

SLOT_C_TOKENS = [
    "const RangedBoundVarClass* const cls = ranged_bound_var_class_for(spec.ctor_rva);",
    "cls->final_vptr != spec.vptr_tag",
    "cls->mid_vptr != spec.mid_vptr_tag",
    "cls->fn_vtbl != spec.functor_tag",
    "cls->dtor_rva != spec.dtor_rva",
    "args.fn_vptr = spec.functor_tag;",
    "init_ranged_bound_var_a4f4d0(",
    "Slot309Hooks default_slot309_hooks() noexcept",
    "hooks.var_ctor = &slot309_var_ctor_native;",
    "if (!hooks.var_ctor(object, spec, concmd)) return stages.ctor;",
]

SLOT_H_TOKENS = [
    "bool (*var_ctor)(Slot309VarObject& object, const Slot309VarSpec& spec,",
    "[[nodiscard]] bool slot309_var_ctor_native(Slot309VarObject& object, const Slot309VarSpec& spec,",
    "[[nodiscard]] Slot309Hooks default_slot309_hooks() noexcept;",
    "std::uint64_t leaf_rva;",
    "std::uint64_t mid_vptr_tag;",
    "struct Slot309VarObject {\n    ConCmdRangedVar var{};\n};",
    '#include "ranged_bound_var_a4f4d0.hpp"',
]

STARTUP_TOKENS = [
    '{0xa505e0, 309, [](StartupState& s) noexcept {',
    "const Slot309Hooks hooks = default_slot309_hooks();",
    "s.slot309_result = init_slot309_a505e0(s.slot309, s.concmd, s.debug_menu, hooks);",
    "if (!s.slot309_result.ok) s.init_failed = true;",
]
STARTUP_H_TOKENS = ['#include "ctor_slot_309_a505e0.hpp"', "Slot309Objects slot309{};",
                    "Slot309Result slot309_result{};"]

TEST_TOKENS = ["section_constants", "section_build_three", "section_value_cell", "section_args_copy",
               "section_refusals_write_nothing", "section_registration",
               "kRangedBoundFunctorSlotCalls == 11",
               "r.functor_slot_calls == kRangedBoundFunctorSlotCalls"]
SLOT_TEST_TOKENS = ["section_native_build", "section_native_var_refused",
                    "default_slot309_hooks()", "slot309_var_ctor_native"]
COVERAGE_TOKENS = ["slots.size() == 450", "first_unrecovered_constructor_index() == 312",
                   "first_unrecovered_constructor_rva() == 0xa46130",
                   "slots.size() + 123 == kOriginalCtorCount",
                   "st.slot309_result.ok"]
CMAKE_TOKENS = ["core/ranged_bound_var_a4f4d0.cpp", "core/ctor_slot_309_a505e0.cpp",
                "add_test(NAME ranged_bound_var_a4f4d0 COMMAND test_ranged_bound_var_a4f4d0)",
                "add_test(NAME ctor_slot_309 COMMAND test_ctor_slot_309)"]


def fde_range(e: Elf, initial: int, want_reclen: int) -> int:
    """address_range of the FDE whose initial_location is `initial` (PT_GNU_EH_FRAME datarel table)."""
    header = e.raw(EH_FRAME_HDR, 12)
    require(header[0] == 1 and header[1] == 0x1B and header[2] == 3 and header[3] == 0x3B,
            "eh_frame_hdr version/encodings")
    count = struct.unpack("<I", header[8:12])[0]
    table = e.img.read(EH_FRAME_HDR + 12, count * 8)
    for i in range(count):
        init, fde = struct.unpack_from("<ii", table, 8 * i)
        if init + EH_FRAME_HDR != initial:
            continue
        fde += EH_FRAME_HDR
        record = e.read(fde, 16)
        require(struct.unpack("<I", record[0:4])[0] == want_reclen,
                f"FDE record length for 0x{initial:x}")
        location = fde + 8 + struct.unpack("<i", record[8:12])[0]
        require(location == initial, f"FDE initial_location 0x{location:x}")
        rng = struct.unpack("<I", record[12:16])[0]
        require(struct.unpack("<I", record[4:8])[0] != 0, "FDE CIE pointer")
        return rng
    raise SystemExit(f"FAIL no FDE for 0x{initial:x} in eh_frame_hdr")


def mask_fields(body: bytes, fields) -> bytes:
    data = bytearray(body)
    for off, size in fields:
        data[off:off + size] = b"\0" * size
    return bytes(data)


def check_elf(e: Elf) -> None:
    # --- the family: extents, digests, prologues -------------------------------------------------
    for i, (wrapper, leaf, mid, fn, final, fn_destroy, dtor, non_deleting) in enumerate(CLASSES):
        wbody = e.read(wrapper, WRAPPER_SIZE)
        require(fde_range(e, wrapper, FDE_RECLEN_BODY) == WRAPPER_SIZE, f"wrapper 0x{wrapper:x} FDE range")
        if not e.skip_hash:
            require(digest(wbody) == WRAPPER_SHA[i], f"wrapper 0x{wrapper:x} sha256")
        require(wbody[: len(WRAPPER_PROLOGUE) // 2].hex() == WRAPPER_PROLOGUE,
                f"wrapper 0x{wrapper:x} prologue")

        lbody = e.read(leaf, LEAF_SIZE)
        require(fde_range(e, leaf, FDE_RECLEN_BODY) == LEAF_SIZE, f"leaf 0x{leaf:x} FDE range")
        if not e.skip_hash:
            require(digest(lbody) == LEAF_SHA[i], f"leaf 0x{leaf:x} sha256")
        require(lbody[: len(LEAF_PROLOGUE) // 2].hex() == LEAF_PROLOGUE, f"leaf 0x{leaf:x} prologue")

        # The leaf ends exactly where its class's non-deleting destructor starts.
        require(leaf + LEAF_SIZE == non_deleting, f"leaf 0x{leaf:x} + 0x160 == non-deleting dtor")
        # The two destructors are shared identity tags, so their digests stay active through the
        # mutation self-test (`always=True` semantics, like every other shared callee in this tree).
        dbody = e.read(dtor, DTOR_SIZE)
        require(fde_range(e, dtor, FDE_RECLEN_DTOR) == DTOR_SIZE, f"dtor 0x{dtor:x} FDE range")
        require(digest(dbody) == DTOR_SHA[i], f"dtor 0x{dtor:x} sha256")
        ndbody = e.read(non_deleting, NON_DELETING_DTOR_SIZE)
        require(digest(ndbody) == NON_DELETING_SHA[i], f"non-deleting dtor 0x{non_deleting:x} sha256")
        e.bytes_at(non_deleting, "554889e5", f"non-deleting dtor 0x{non_deleting:x} prologue")
        require(fde_range(e, non_deleting, FDE_RECLEN_DTOR) == NON_DELETING_DTOR_SIZE,
                f"non-deleting dtor 0x{non_deleting:x} FDE range")

        # --- indirect calls and branches (documented offsets; the digest above pins the rest) -----
        for off, encoding, what in WRAPPER_INDIRECT:
            e.bytes_at(wrapper + off, encoding, f"wrapper {what}")
        for off, encoding, what in WRAPPER_BRANCHES:
            e.bytes_at(wrapper + off, encoding, f"wrapper {what}")
        for off, encoding, what in LEAF_INDIRECT:
            e.bytes_at(leaf + off, encoding, f"leaf {what}")
        for off, encoding, what in LEAF_BRANCHES:
            e.bytes_at(leaf + off, encoding, f"leaf {what}")

        # --- the three direct calls of a wrapper / two of a leaf ---------------------------------
        require(e.call(wrapper + 0x157) == leaf, f"wrapper 0x{wrapper:x} calls its own leaf")
        require(e.call(wrapper + 0x187) == REGISTER, f"wrapper 0x{wrapper:x} calls 0x20d60")
        require(e.call(wrapper + 0x1AE) == STACK_CHK_FAIL, f"wrapper 0x{wrapper:x} canary fail call")
        require(e.lea(wrapper + 0x22) + 0x10 == final, f"wrapper 0x{wrapper:x} final address point")
        require(e.lea(wrapper + 0x5A) == fn, f"wrapper 0x{wrapper:x} functor class lea")
        require(e.rip(wrapper + 0x1B, 7, 3) == GUARD_CELL, f"wrapper 0x{wrapper:x} guard cell")
        require(e.rip(wrapper + 0x18C, 7, 3) == GUARD_CELL, f"wrapper 0x{wrapper:x} epilogue guard")
        require(e.call(leaf + 0x36) == BASE_CTOR, f"leaf 0x{leaf:x} calls 0x20af0")
        require(e.call(leaf + 0x15A) == STACK_CHK_FAIL, f"leaf 0x{leaf:x} canary fail call")
        require(e.lea(leaf + 0x3B) + 0x10 == mid, f"leaf 0x{leaf:x} mid address point")
        require(e.lea(leaf + 0x46) == fn, f"leaf 0x{leaf:x} functor class lea")
        require(e.rip(leaf + 0x11, 7, 3) == GUARD_CELL, f"leaf 0x{leaf:x} guard cell")
        require(e.rip(leaf + 0x13B, 7, 3) == GUARD_CELL, f"leaf 0x{leaf:x} epilogue guard")
        # The branch that leaves the body lands on the __stack_chk_fail call.
        for body, base, off, target_off in ((wbody, wrapper, 0x19A, 0x1AE), (lbody, leaf, 0x149, 0x15A)):
            rel = struct.unpack("<b", body[off + 1:off + 2])[0]
            require(base + off + 2 + rel == base + target_off,
                    f"branch at 0x{base + off:x} targets the __stack_chk_fail call")

        # --- the class identities -----------------------------------------------------------------
        require(e.reloc_target(final + 0x00) == dtor, f"final 0x{final:x} slot 0 == dtor")
        require(e.reloc_target(final + 0x08) == non_deleting,
                f"final 0x{final:x} slot 1 == non-deleting dtor")
        require(e.reloc_target(final + 0x10) == REGISTER, f"final 0x{final:x} slot 2 == 0x20d60")
        e.bytes_at(e.reloc_target(final + 0x18), "b801000000c3", f"final 0x{final:x} slot 3 returns 1")
        e.bytes_at(e.reloc_target(final + 0x30), "b801000000c3", f"final 0x{final:x} slot 6 returns 1")

        # --- the functor class ------------------------------------------------------------------
        copy, clone, move, destroy = FUNCTOR_SLOTS[i]
        for slot in range(4):
            require(e.reloc_target(fn + 8 * slot) == (copy, clone, move, destroy)[slot],
                    f"functor class 0x{fn:x} slot {slot}")
        e.bytes_at(destroy, "c3", f"functor destroy 0x{destroy:x} is a bare ret")
        require(fn_destroy == destroy, "class table functor_destroy_rva")
        # copy: {dst+0x10 = src+0x10; dst+0x20 = class tag} with the tag loaded by a lea; clone loads
        # the tag first; move-assign copies the payload only.
        e.bytes_at(copy, "488b4610", f"functor copy 0x{copy:x} loads the payload")
        require(e.lea(copy + 4) == fn, f"functor copy 0x{copy:x} stores the class tag")
        e.bytes_at(copy + 0xB, "4889471048894f20c3", f"functor copy 0x{copy:x} stores")
        require(e.lea(clone) == fn, f"functor clone 0x{clone:x} stores the class tag")
        e.bytes_at(clone + 7, "48894720", f"functor clone 0x{clone:x} stores the tag at +0x20")
        e.bytes_at(move, "488b461048894710c3", f"functor move-assign 0x{move:x} body")

    # --- the shared template: equal after masking the identity fields ---------------------------
    wrappers = [e.read(w[0], WRAPPER_SIZE) for w in CLASSES]
    leaves = [e.read(w[1], LEAF_SIZE) for w in CLASSES]
    masked_wrapper = [mask_fields(b, WRAPPER_FIELDS) for b in wrappers]
    masked_leaf = [mask_fields(b, LEAF_FIELDS) for b in leaves]
    require(len(WRAPPER_RELOC) == 6 and len(LEAF_RELOC) == 6, "relocation field counts")
    require(masked_wrapper[0] == masked_wrapper[1] == masked_wrapper[2],
            "the three wrappers are the same bytes outside the five relocation immediates")
    require(masked_leaf[0] == masked_leaf[1] == masked_leaf[2],
            "the three leaves are the same bytes outside the four relocation immediates")
    # ... and the masked fields are exactly the identity operands, nothing else.
    for i, (wrapper, leaf, mid, fn, _final, _fd, _dtor, _nd) in enumerate(CLASSES):
        for off, size, _what in WRAPPER_RELOC:
            require(wrappers[i][off:off + size] != b"\0" * size, f"wrapper field 0x{off:x} is a real operand")
        for off, size, _what in LEAF_RELOC:
            require(leaves[i][off:off + size] != b"\0" * size, f"leaf field 0x{off:x} is a real operand")
        require(e.lea(wrapper + 0x22) + 0x10 == _final, f"wrapper 0x{wrapper:x} final address point")
        require(e.lea(leaf + 0x3B) + 0x10 == mid, f"leaf 0x{leaf:x} mid address point")

    # --- the caller: startup table row 309 and its three calls ----------------------------------
    require(TABLE_TOP - 8 * SLOT309_INDEX == 0x2DD6738, "table row arithmetic")
    require(e.u64(0x2DD6738) == SLOT309_RVA, "constructor-table entry 309 -> 0xa505e0")
    for site, target in ((0xA50720, 0xA4F4D0), (0xA50AD8, 0xA4FEC0), (0xA50C57, 0xA50280)):
        require(e.call(site) == target, f"slot 309 call at 0x{site:x} -> 0x{target:x}")
    # The slot's own argument-block tag equals the instantiation's functor class at all three sites.
    for site, fn in ((0xA50684, 0x15D1B78), (0xA50A35, 0x15D1C30), (0xA50BBE, 0x15D1CC8)):
        require(e.lea(site) == fn, f"slot 309 argument tag at 0x{site:x}")


def check_native(texts: dict[str, str]) -> None:
    header = texts["header"]
    for name, value in CONSTANTS.items():
        require(const_value(header, name) == value, f"native constant {name}")
    for row in HEADER_ROWS:
        require(row in header, f"header class row {row}")
    positions = [texts["source"].index(token) if token in texts["source"] else -1
                 for token in SOURCE_ORDER]
    require(-1 not in positions, "every store token is present in the implementation")
    require(positions == sorted(positions) and len(set(positions)) == len(positions),
            "native store sequence is in body order")
    for token in SLOT_C_TOKENS:
        require(token in texts["slot_c"], f"slot 309 source token {token!r}")
    for token in SLOT_H_TOKENS:
        require(token in texts["slot_h"], f"slot 309 header token {token!r}")
    for row in SPEC_ROWS:
        require(row in texts["slot_h"], f"slot 309 spec row {row}")
    for token in STARTUP_TOKENS:
        require(token in texts["startup_c"], f"startup row token {token!r}")
    for token in STARTUP_H_TOKENS:
        require(token in texts["startup_h"], f"startup header token {token!r}")
    for token in TEST_TOKENS:
        require(token in texts["test"], f"model test token {token!r}")
    for token in SLOT_TEST_TOKENS:
        require(token in texts["slot_test"], f"slot test token {token!r}")
    for token in COVERAGE_TOKENS:
        require(token in texts["coverage"], f"coverage token {token!r}")
    for token in CMAKE_TOKENS:
        require(token in texts["cmake"], f"cmake token {token!r}")
    # The production row is registered exactly once and in index order between 308 and 310.
    match = re.search(r"static constexpr CtorSlot kSlots\[\]\s*=\s*\{(.*?)\n\s*\};",
                      texts["startup_c"], re.DOTALL)
    require(match is not None, "cannot find the recovered constructor array")
    rows = [(int(rva, 16), int(i)) for rva, i in re.findall(
        r"^\s*\{\s*(0x[0-9a-fA-F]+)\s*,\s*(\d+)\s*,\s*\[", match.group(1), re.MULTILINE)]
    require((SLOT309_RVA, SLOT309_INDEX) in rows, "missing registered row 309 / 0xa505e0")
    indices = [i for _, i in rows]
    require(indices == sorted(indices) and len(set(indices)) == len(indices),
            "registered rows are in original index order")
    require(308 in indices and 309 in indices and 310 in indices, "row 309 sits between 308 and 310")


def read_native() -> dict[str, str]:
    return {key: (ROOT / path).read_text() for key, path in NATIVE.items()}


def run(e: Elf, texts: dict[str, str]) -> None:
    check_elf(e)
    check_native(texts)


def expect_failure(label: str, fn) -> None:
    try:
        fn()
    except SystemExit as error:
        require(str(error).startswith("FAIL"), f"mutant {label}: unexpected exit {error!r}")
        return
    raise SystemExit(f"FAIL mutant survived: {label}")


def mutation_self_test(e: Elf, texts: dict[str, str]) -> tuple[int, int]:
    e.recording = True
    check_elf(e)
    e.recording = False
    pins = sorted(e.pins)
    e.skip_hash = True
    ran = survivors = 0
    for addr, size in pins:
        for byte_addr in sorted({addr, addr + size - 1}):
            e.overlay = {}
            pristine = e.raw(byte_addr, 1)[0]
            e.overlay = {byte_addr: pristine ^ 0x01}
            ran += 1
            try:
                check_elf(e)
            except SystemExit:
                continue
            survivors += 1
            print(f"mutation survived at 0x{byte_addr:x} (pin 0x{addr:x}+{size})", file=sys.stderr)
    e.overlay = {}
    e.skip_hash = False
    require(survivors == 0, f"{survivors} ELF mutations survived")

    native_ran = 0
    for key, tokens in (("header", list(CONSTANTS) + HEADER_ROWS), ("source", SOURCE_ORDER),
                        ("slot_c", SLOT_C_TOKENS), ("slot_h", SLOT_H_TOKENS + SPEC_ROWS),
                        ("startup_c", STARTUP_TOKENS), ("startup_h", STARTUP_H_TOKENS),
                        ("test", TEST_TOKENS), ("slot_test", SLOT_TEST_TOKENS),
                        ("coverage", COVERAGE_TOKENS), ("cmake", CMAKE_TOKENS)):
        for token in tokens:
            if token not in texts[key]:
                continue
            mutated = dict(texts)
            mutated[key] = texts[key].replace(token, "\0MUTATED\0")
            expect_failure(f"{key} token {token!r}", lambda m=mutated: check_native(m))
            native_ran += 1
    for name, value in CONSTANTS.items():
        pattern = re.compile(rf"({re.escape(name)}\s*=\s*)(0x[0-9A-Fa-f]+|\d+)(u?\s*;)")
        mutated = dict(texts)
        mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}",
                                        texts["header"], count=1)
        require(mutated["header"] != texts["header"], f"constant {name} pattern did not match")
        expect_failure(f"constant {name}", lambda m=mutated: check_native(m))
        native_ran += 1
    return ran, native_ran


def main() -> int:
    arguments = sys.argv[1:]
    if not (ROOT / "local/re/eboot.elf").is_file():
        print("SKIP check_console_var_ctors: local/re/eboot.elf not present")
        return 0
    e = Elf()
    texts = read_native()
    if "--tamper" in arguments:
        mode = arguments[arguments.index("--tamper") + 1] if len(arguments) > arguments.index("--tamper") + 1 else "elf"
        if mode == "elf":
            e.overlay = {0xA4F6D9: e.raw(0xA4F6D9, 1)[0] ^ 0x01}   # leaf 1 functor-class immediate
        else:
            texts = dict(texts)
            texts["header"] = texts["header"].replace("kRangedBoundWrapperSize", "kTampered", 1)
    e.recording = True
    run(e, texts)
    e.recording = False
    note = ""
    if "--no-mutate" not in arguments and "--tamper" not in arguments:
        elf_runs, native_runs = mutation_self_test(e, texts)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print("OK check_console_var_ctors: the three 0xa4f4d0 / 0xa4fec0 / 0xa50280 wrappers (436 B each) "
          "and their 0xa4f690 / 0xa50080 / 0xa50440 leaves (352 B each) are one template modulo "
          f"6 + 6 relocation immediates; 6 / 5 indirect functor dispatches, 2 branches each, canary via "
          f"0x15fe518 -> 0xf8f3a0; slot 309 row 0xa505e0 registered through default_slot309_hooks(); "
          f"guest bytes NOT executed{note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
