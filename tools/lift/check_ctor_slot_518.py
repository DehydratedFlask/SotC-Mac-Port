#!/usr/bin/env python3
"""ELF tie for startup constructor index 518 (RVA 0x1a57d0, 527 B): one bounded float console variable
(`vgAmbientSoundSpacing`, range [1.0, 20.0], value 7.0, no item) plus a bounded STATIC-FLOAT TAIL — a data row of
the CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 cluster model with one extra pinned fragment.

What this checker adds to the shared lib (tools/lift/debug_page_batch_lib.py):
  * the caller argument-functor tag 0x1598a50: its four .data.rel.ro relocations resolve to the float bodies
    0xc4a0/0xc4c0/0xc4e0/0xc4f0 and the last is a bare `ret` (the lib's functor_tags check, so the tag is the same
    float tag as the rest of the batch and `callq *0x18(%rax)` is the bare-ret destroy);
  * the tail: every store immediate (the eight literal dwords at 0x2ee66c0..0x2ee66dc), the three .rodata float
    constants 0xfadb60/64/68 (0.5f, 0.866f, 1.0f), the file-backed 8.5f cache at 0x16048a0, the rip targets of the
    eleven cells, that the cells are .bss (not file-backed) and directly follow the variable (0x2ee65e8 + 0xd8), and
    the vmulss / vaddss / vdivss chain, whose three results this checker recomputes with an INDEPENDENT struct-based
    float32 model (each operation rounded once to single precision) and ties to the native constants.

The cache is the one inference: its only other code reference is a runtime function at 0x1a39a6; no startup row writes
it. That is a static enumeration of `lea 0x16048a0`, recorded in the native header, not an execution.

Offline and read-only. Skips cleanly (exit 0) when the private inputs are absent. `--tamper elf|native` must exit 1.
"""

from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import debug_page_batch_lib as lib  # noqa: E402
from slot_pin_util import require  # noqa: E402


def f32(x: float) -> float:
    return struct.unpack("<f", struct.pack("<f", x))[0]


def bits(x: float) -> int:
    return struct.unpack("<I", struct.pack("<f", x))[0]


def from_bits(b: int) -> float:
    return struct.unpack("<f", struct.pack("<I", b))[0]


# Independent float32 model of the tail (a product of two floats is exact in a double; the single rounding is the
# struct pack; the quotient's double rounding is innocuous for float32 operands).
CACHE_BITS = 0x41080000
HALF_BITS, SCALE_BITS, ONE_BITS = 0x3F000000, 0x3F5DB22D, 0x3F800000
E0 = f32(from_bits(CACHE_BITS) * from_bits(HALF_BITS))
E4 = f32(f32(E0 + E0) * from_bits(SCALE_BITS))
E8 = f32(from_bits(ONE_BITS) / E4)

PAIRS = {   # cell address -> (low dword, high dword), as stored by the literal movl / movabs instructions
    0x2EE66C0: (0x3F800000, 0x3F000000),
    0x2EE66C8: (0x00000000, 0x3F5DB22D),
    0x2EE66D0: (0x3F800000, 0xBF13CE56),
    0x2EE66D8: (0x00000000, 0x3F93CE56),
}
CELLS = [0x2EE66C0, 0x2EE66C8, 0x2EE66D0, 0x2EE66D8, 0x2EE66E0, 0x2EE66E4, 0x2EE66E8]
VAR = 0x2EE65E8
VAR_STRIDE = 0xD8

SPEC = {
    "index": 518,
    "rva": 0x1A57D0,
    "size": 527,                       # FDE [0x1a57d0, 0x1a59df)
    "sha": "172ed60c075ac81fd92073a19a808c8d7a6a256fcffd26976fab7b145bd54829",
    "table_word": 0x2DD60B0,           # 0x2dd70e0 - 518*8
    "fde_addr": 0x1358330,
    "fde_reclen": 36,
    "first_constant": "kSlot518Rva",
    "direct_calls": 6,
    "callees": 5,
    "rip_operands": 20,
    "rip_targets": 19,
    "header": "native/core/ctor_slot_518_1a57d0.hpp",
    "source": "native/core/ctor_slot_518_1a57d0.cpp",
    "test": "native/tests/test_ctor_slot_518.cpp",
    "constants": {
        "kSlot518Rva": 0x1A57D0,
        "kSlot518BodySize": 527,
        "kSlot518EndRva": 0x1A59DF,
        "kSlot518InstructionCount": 103,
        "kSlot518DirectCallSites": 6,
        "kSlot518DirectCallees": 5,
        "kSlot518FunctorSites": 1,
        "kSlot518BranchCount": 1,
        "kSlot518TableTopRva": 0x2DD70E0,
        "kSlot518TableWordRva": 0x2DD60B0,
        "kSlot518TableIndex": 518,
        "kSlot518CanaryCellRva": 0x15FE518,
        "kSlot518CanaryFrameOffset": 0xC0,
        "kSlot518BranchRva": 0x1A59C8,
        "kSlot518BranchTargetRva": 0x1A59D9,
        "kSlot518VarRva": VAR,
        "kSlot518TailBaseRva": 0x2EE66C0,
        "kSlot518CacheCellRva": 0x16048A0,
        "kSlot518VarNameRva": 0xFADB6C,
        "kSlot518VarDescriptionRva": 0xFADBB5,
        "kSlot518VarLoBits": 0x3F800000,
        "kSlot518VarHiBits": 0x41A00000,
        "kSlot518VarValueBits": 0x40E00000,
        "kSlot518TailMulHalfRva": 0xFADB60,
        "kSlot518TailMulScaleRva": 0xFADB64,
        "kSlot518TailOneRva": 0xFADB68,
        "kSlot518TailMulHalfBits": HALF_BITS,
        "kSlot518TailMulScaleBits": SCALE_BITS,
        "kSlot518TailOneBits": ONE_BITS,
        "kSlot518CacheDefaultBits": CACHE_BITS,
        "kSlot518TailE0Bits": bits(E0),
        "kSlot518TailE4Bits": bits(E4),
        "kSlot518TailE8Bits": bits(E8),
    },
    "header_tokens": [
        'kSlot518VarName[] = "vgAmbientSoundSpacing";',
        *[f"kSlot518TailPair{i}Bits[2] = {{0x{lo:08x}u, 0x{hi:08x}u}};" for i, (lo, hi) in enumerate(PAIRS.values())],
        "{kSlot518VarRva, kSlot518VarName, 0x15, kSlot518VarDescription, 0x00, kSlot518VarLoBits,",
        "debug_page::kArgFnVtblSlot518, 0x1a58c1, 0x1a58dd, 0x1a58f7,",
        "0x1a5915}",
        "std::span<const debug_page::FloatVarRow>(kSlot518Vars)",
        "std::span<const debug_page::MenuRow>()",
        "std::span<const debug_page::HashedNameRow>()",
        "std::uint32_t cache_bits = kSlot518CacheDefaultBits;",
        "kSlot518VarRva + debug_page::kVarStride == kSlot518TailBaseRva",
    ],
    "source_order": [
        "basis.pair1[0] = kSlot518TailPair1Bits[0];",
        "basis.pair1[1] = kSlot518TailPair1Bits[1];",
        "basis.pair0[0] = kSlot518TailPair0Bits[0];",
        "basis.pair0[1] = kSlot518TailPair0Bits[1];",
        "basis.pair2[0] = kSlot518TailPair2Bits[0];",
        "basis.pair2[1] = kSlot518TailPair2Bits[1];",
        "basis.pair3[0] = kSlot518TailPair3Bits[0];",
        "basis.pair3[1] = kSlot518TailPair3Bits[1];",
        "const float spacing = f32(cache_bits) * f32(kSlot518TailMulHalfBits);",
        "basis.spacing = bits_of(spacing);",
        "const float pitch = (spacing + spacing) * f32(kSlot518TailMulScaleBits);",
        "basis.pitch = bits_of(pitch);",
        "basis.inverse = bits_of(f32(kSlot518TailOneBits) / pitch);",
        "const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1), std::span<DebugMenuItem>(),",
        "out.page = debug_page::init_debug_page(kSlot518Script, view, concmd, menu, hooks);",
        "if (out.page.vars_built == 1 && out.page.atexit_records == 1) {",
        "apply_ambient_spacing_tail(objects.basis, objects.cache_bits);",
    ],
    "test_tokens": [
        "section_constants", "section_native_build", "section_tail_follows_the_cache",
        "section_shape_refused", "section_register_refused", "section_unpinned_tag_refused", "section_canary",
        "r.page.atexit_records == 1 && r.page.functors_destroyed == 1",
        "b.spacing == kSlot518TailE0Bits && b.pitch == kSlot518TailE4Bits && b.inverse == kSlot518TailE8Bits",
        "records[0].dtor_rva == 0x20f0",
        "region_poison(objects.basis, 0, sizeof objects.basis)",
    ],
    "cmake_tokens": [
        "core/ctor_slot_518_1a57d0.cpp",
        "add_test(NAME ctor_slot_518 COMMAND test_ctor_slot_518)",
    ],
    "calls": [
        (0x1A58C1, 0xC300),
        (0x1A58CD, 0xF590),     # f590(description)
        (0x1A58F7, 0x20D60),
        (0x1A58FF, 0xF590),     # f590(name)
        (0x1A5915, 0xF8F390),   # __cxa_atexit(0x20f0, var)
        (0x1A59D9, 0xF8F3A0),   # __stack_chk_fail (branch target only)
    ],
    "rips": [
        (0x1A57E8, 7, 3, 0x15FE518),   # __stack_chk_guard GOT cell
        (0x1A57EF, 7, 3, 0x15942A0),   # 0x15942b0 - 0x10
        (0x1A57F6, 7, 3, VAR),         # the variable object
        (0x1A57FD, 7, 3, 0xFADB6C),    # name data
        (0x1A5811, 7, 3, 0x1598A50),   # caller argument functor tag
        (0x1A584A, 7, 3, 0xFADBB5),    # description data (empty)
        (0x1A5904, 7, 3, 0x20F0),      # the variable destructor
        (0x1A590B, 7, 3, 0x1604218),   # __dso_handle
        (0x1A591A, 7, 3, 0x2EE66C8),   # tail cell pair1
        (0x1A5921, 7, 3, 0x2EE66C0),   # tail cell pair0
        (0x1A5928, 8, 4, 0xFADB68),    # 1.0f
        (0x1A5930, 7, 3, 0x2EE66D0),   # tail cell pair2
        (0x1A5937, 7, 3, 0x2EE66D8),   # tail cell pair3
        (0x1A594B, 7, 3, 0x16048A0),   # the 8.5f cache
        (0x1A595F, 7, 3, 0x2EE66E0),   # tail cell spacing
        (0x1A5980, 7, 3, 0x15FE518),   # the epilogue guard re-read
        (0x1A598B, 8, 4, 0xFADB60),    # 0.5f
        (0x1A599B, 8, 4, 0xFADB64),    # 0.866f
        (0x1A59A3, 7, 3, 0x2EE66E4),   # tail cell pitch
        (0x1A59B2, 7, 3, 0x2EE66E8),   # tail cell inverse
    ],
    "bytes": [
        (0x1A57DD, "4883e4e0", "and rsp,-0x20"),
        (0x1A57E1, "4881ece0000000", "sub rsp,0xe0"),
        (0x1A5825, "4c8d442418", "r8 = &max (0x18(%rsp))"),
        (0x1A583D, "48898424c0000000", "canary -> 0xc0(%rsp)"),
        (0x1A5851, "48c744242815000000", "name length 0x15"),
        (0x1A587A, "c744241c0000803f", "min = 1.0f at 0x1c(%rsp)"),
        (0x1A5882, "c74424180000a041", "max = 20.0f at 0x18(%rsp)"),
        (0x1A58B9, "488d4c241c", "rcx = &min (0x1c(%rsp))"),
        (0x1A58DD, "ff5018", "callq *0x18(%rax)"),
        (0x1A58E6, "c783d00000000000e040", "value store +0xd0 = 7.0f"),
        (0x1A58F0, "c743680000e040", "default store +0x68 = 7.0f"),
        # ---- the static-float tail ----
        (0x1A593E, "c70200000000", "[0x2ee66c8] = 0"),
        (0x1A5944, "c742042db25d3f", "[0x2ee66cc] = 0x3f5db22d"),
        (0x1A5952, "c7000000803f", "[0x2ee66c0] = 1.0f"),
        (0x1A5958, "c740040000003f", "[0x2ee66c4] = 0.5f"),
        (0x1A5966, "c7010000803f", "[0x2ee66d0] = 1.0f"),
        (0x1A596C, "c7410456ce13bf", "[0x2ee66d4] = 0xbf13ce56"),
        (0x1A5973, "48b90000000056ce933f", "movabs rcx, 0x3f93ce5600000000"),
        (0x1A597D, "48890e", "[0x2ee66d8] = rcx"),
        (0x1A5987, "c5fa1002", "vmovss xmm0, (%rdx) (the cache)"),
        (0x1A598B, "c5fa5905", "vmulss 0.5f"),
        (0x1A5993, "c5fa1100", "vmovss (%rax), xmm0 -> [0x2ee66e0]"),
        (0x1A5997, "c5fa58c0", "vaddss xmm0, xmm0"),
        (0x1A599B, "c5fa5905", "vmulss 0.866f"),
        (0x1A59AA, "c5fa1100", "vmovss (%rax), xmm0 -> [0x2ee66e4]"),
        (0x1A59AE, "c5f25ec0", "vdivss xmm1(1.0f) / xmm0"),
        (0x1A59B9, "c5fa1100", "vmovss (%rax), xmm0 -> [0x2ee66e8]"),
        (0x1A59BD, "488b01", "movq (%rcx), %rax (guard re-read)"),
        (0x1A59C0, "483b8424c0000000", "cmpq 0xc0(%rsp), %rax"),
        (0x1A59C8, "750f", "jne 0x1a59d9 (canary fail)"),
        (0x1A59CA, "488d65d8", "leaq -0x28(%rbp), %rsp"),
        (0x1A59CE, "5b415c415d415e415f5dc3", "pops + retq"),
        (0x1A59D9, "e8c299de009090", "callq __stack_chk_fail + the body's two nops"),
    ],
    "branch": (0x1A59C8, 0x1A59D9),
    "functor_sites": [0x1A58DD],
    "functor_tags": [0x1598A50],
    "atexit": [
        (0x1A5915, 0x20F0, VAR),
    ],
    "strings": [
        (0xFADB6C, b"vgAmbientSoundSpacing"),
        (0xFADBB5, b""),
    ],
}


def u32(e, addr: int) -> int:
    return struct.unpack("<I", e.read(addr, 4))[0]


def check_tail_elf(e, spec: dict) -> None:
    # The cells are .bss (no file bytes) and follow the variable exactly.
    require(VAR + VAR_STRIDE == CELLS[0], "the tail cells directly follow the 0xd8-byte variable")
    for cell in CELLS:
        try:
            e.raw(cell, 4)
        except ValueError:
            continue
        raise SystemExit(f"FAIL tail cell 0x{cell:x} is unexpectedly file-backed")

    # The store immediates, read from the instruction bytes.
    pair_sites = {   # cell -> (site of low-dword imm, site of high-dword imm)
        0x2EE66C8: (0x1A593E + 2, 0x1A5944 + 3),
        0x2EE66C0: (0x1A5952 + 2, 0x1A5958 + 3),
        0x2EE66D0: (0x1A5966 + 2, 0x1A596C + 3),
    }
    for cell, (lo_site, hi_site) in pair_sites.items():
        require((u32(e, lo_site), u32(e, hi_site)) == PAIRS[cell], f"pair at 0x{cell:x}")
    movabs = struct.unpack("<Q", e.read(0x1A5973 + 2, 8))[0]
    require((movabs & 0xFFFFFFFF, movabs >> 32) == PAIRS[0x2EE66D8], "movabs pair at 0x2ee66d8")

    # The cache and the three rodata constants, then the arithmetic chain recomputed from the ELF's own values.
    cache = u32(e, 0x16048A0)
    require(cache == CACHE_BITS, "the file-backed cache cell holds 8.5f")
    half, scale, one = u32(e, 0xFADB60), u32(e, 0xFADB64), u32(e, 0xFADB68)
    require((half, scale, one) == (HALF_BITS, SCALE_BITS, ONE_BITS), "0.5f / 0.866f / 1.0f constants")
    spacing = f32(from_bits(cache) * from_bits(half))
    pitch = f32(f32(spacing + spacing) * from_bits(scale))
    inverse = f32(from_bits(one) / pitch)
    require((bits(spacing), bits(pitch), bits(inverse)) == (bits(E0), bits(E4), bits(E8)),
            "the ELF-derived tail results equal the pinned model")


def check_native_tail(texts: dict, spec: dict) -> None:
    # The model's results (derived above from the ELF inputs) are the header constants; the pair literals too.
    header = texts["header"]
    for name, value in (("kSlot518TailE0Bits", bits(E0)), ("kSlot518TailE4Bits", bits(E4)),
                        ("kSlot518TailE8Bits", bits(E8))):
        require(lib.const_value(header, name) == value, f"native constant {name} equals the float32 model")
    for i, (lo, hi) in enumerate(PAIRS.values()):
        require(f"kSlot518TailPair{i}Bits[2] = {{0x{lo:08x}u, 0x{hi:08x}u}};" in header, f"native pair {i}")


_base_check_elf = lib.check_elf
_base_check_native = lib.check_native


def check_elf(e, spec):
    _base_check_elf(e, spec)
    check_tail_elf(e, spec)


def check_native(texts, spec):
    _base_check_native(texts, spec)
    check_native_tail(texts, spec)


lib.check_elf = check_elf
lib.check_native = check_native

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, sys.argv[1:]))
