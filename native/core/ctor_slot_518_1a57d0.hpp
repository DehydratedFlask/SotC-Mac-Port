// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): startup constructor index 518 / RVA 0x1a57d0 (FDE 527 B
// [0x1a57d0, 0x1a59df), 103 instructions), ELF sha256 d0723afb...5861d3. Offline analysis only.
//
// A DATA row of core/debug_page_var_item (one bounded float variable, no menu item, no leading record) PLUS a
// bounded static-float tail that is not part of the shared template. Every fact below is pinned by
// tools/lift/check_ctor_slot_518.py.
//
// Characterisation (from the pinned bytes):
//   * the startup table word at 0x2dd70e0 - 518*8 = 0x2dd60b0 holds 0x1a57d0;
//   * prologue `push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0xe0`, guard
//     `*[0x15fe518]` at 0xc0(%rsp), exactly one conditional branch (0x1a59c8 `jne` -> 0x1a59d9, the
//     __stack_chk_fail call), exactly one indirect call (0x1a58dd `callq *0x18(%rax)`);
//   * 6 direct call sites over 5 callees: 0xc300 x1, 0xf590 x2, 0x20d60 x1, __cxa_atexit x1,
//     __stack_chk_fail x1;
//   * the variable `vgAmbientSoundSpacing` at 0x2ee65e8 (0xd8 B), value cell = its own +0xd0, range
//     [1.0, 20.0], value = default = 7.0, no description;
//   * the caller argument-functor tag 0x1598a50. Its four .data.rel.ro slots are the float bodies
//     0xc4a0/0xc4c0/0xc4e0/0xc4f0 (relocation-pinned by the checker), i.e. the SAME bodies the batch's other
//     tags resolve to, so the site's `callq *0x18(%rax)` is the bare-ret destroy. core/lane_m6_shapes already
//     named this tag kFloatSiteTag_518; the batch model now accepts it as kArgFnVtblSlot518.
//   * one __cxa_atexit record: (0x1a5915, dtor 0x20f0, var 0x2ee65e8) with __dso_handle 0x1604218.
//
// ------------------------------------ the static-float tail ------------------------------------
// After the atexit record (and before the canary compare) the body stores eleven dwords into the .bss cells
// 0x2ee66c0..0x2ee66ec that directly follow the variable (0x2ee65e8 + 0xd8 = 0x2ee66c0):
//
//   [0x2ee66c0] = {1.0, 0.5}            [0x2ee66d0] = {1.0, -0.5773671865}
//   [0x2ee66c8] = {0.0, 0.8659999967}   [0x2ee66d8] = {0.0, 1.1547343731}   (movabs 0x3f93ce5600000000)
//   cache      = the float at 0x16048a0 (file-backed .data, 8.5f = 0x41080000)
//   [0x2ee66e0] = cache * 0.5f                       (0xfadb60)
//   [0x2ee66e4] = (e0 + e0) * 0.8659999967f           (0xfadb64)
//   [0x2ee66e8] = 1.0f / [0x2ee66e4]                 (0xfadb68)
//
// The pairs are a fixed 3-axis hexagonal basis (cos/sin of 60 degrees and the 1/sin(60 degrees) scale), the
// three computed cells are the cache-derived spacing, its scaled pitch and the reciprocal. The IEEE single
// arithmetic is reproduced with plain float operations (one rounding each, no fused ops: the add feeds a
// multiply, so there is nothing to contract), and the checker recomputes the same three results with an
// independent struct-based float32 model.
//
// MODELLED vs HYPOTHESIS: the cache cell 0x16048a0 holds 8.5f in the ELF's file-backed .data. The ONLY other
// reference to it in the executable's code is a runtime function at 0x1a39a6 (`lea 0x16048a0`), which is not
// a startup-table row, so no earlier startup constructor is known to change it. That is a static inference
// (every `lea` of 0x16048a0 in the code was enumerated), not an execution; the native model therefore takes the
// cache as an explicit field with the ELF default and the tail reads that field, so a host can state a different
// value. The tail is gated on the variable block having completed (its atexit record exists), which is the
// original's straight-line order.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

#include "debug_page_var_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot518Rva = 0x1a57d0;
inline constexpr std::size_t kSlot518BodySize = 527;               // FDE [0x1a57d0, 0x1a59df)
inline constexpr std::uint64_t kSlot518EndRva = 0x1a59df;
inline constexpr std::size_t kSlot518InstructionCount = 103;
inline constexpr std::size_t kSlot518DirectCallSites = 6;
inline constexpr std::size_t kSlot518DirectCallees = 5;
inline constexpr std::size_t kSlot518FunctorSites = 1;
inline constexpr std::size_t kSlot518BranchCount = 1;
inline constexpr std::uint64_t kSlot518TableTopRva = 0x2dd70e0;    // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot518TableWordRva = 0x2dd60b0;   // 0x2dd70e0 - 518*8
inline constexpr std::uint32_t kSlot518TableIndex = 518;

// Frame / guard.
inline constexpr std::uint64_t kSlot518CanaryCellRva = 0x15fe518;
inline constexpr std::uint32_t kSlot518CanaryFrameOffset = 0xc0;
inline constexpr std::uint64_t kSlot518BranchRva = 0x1a59c8;       // jne 0x1a59d9 (the only branch)
inline constexpr std::uint64_t kSlot518BranchTargetRva = 0x1a59d9; // callq __stack_chk_fail

// Objects (identity addresses; the stand-ins live in Slot518Objects at the same strides).
inline constexpr std::uint64_t kSlot518VarRva = 0x2ee65e8;
inline constexpr std::uint64_t kSlot518TailBaseRva = 0x2ee66c0;    // var + 0xd8
inline constexpr std::uint64_t kSlot518CacheCellRva = 0x16048a0;   // file-backed .data

// Strings: identity address + pinned length (the native views are literals).
inline constexpr std::uint64_t kSlot518VarNameRva = 0xfadb6c;
inline constexpr std::uint64_t kSlot518VarDescriptionRva = 0xfadbb5;   // empty
inline constexpr char kSlot518VarName[] = "vgAmbientSoundSpacing";                  // 0x15
inline constexpr char kSlot518VarDescription[] = "";                                // length 0
static_assert(sizeof kSlot518VarName - 1 == 0x15 && sizeof kSlot518VarDescription - 1 == 0x00);

// Range and value of the variable.
inline constexpr std::uint32_t kSlot518VarLoBits = 0x3f800000u;    // 1.0f   (stored at 0x1c(%rsp))
inline constexpr std::uint32_t kSlot518VarHiBits = 0x41a00000u;    // 20.0f  (stored at 0x18(%rsp))
inline constexpr std::uint32_t kSlot518VarValueBits = 0x40e00000u; // 7.0f

// The static-float tail: the literal dwords, the three constants read from .rodata, the cache default and
// the three derived results (pinned by the checker with an independent float32 model).
inline constexpr std::uint32_t kSlot518TailPair0Bits[2] = {0x3f800000u, 0x3f000000u};   // [0x2ee66c0]
inline constexpr std::uint32_t kSlot518TailPair1Bits[2] = {0x00000000u, 0x3f5db22du};   // [0x2ee66c8]
inline constexpr std::uint32_t kSlot518TailPair2Bits[2] = {0x3f800000u, 0xbf13ce56u};   // [0x2ee66d0]
inline constexpr std::uint32_t kSlot518TailPair3Bits[2] = {0x00000000u, 0x3f93ce56u};   // [0x2ee66d8]
inline constexpr std::uint64_t kSlot518TailMulHalfRva = 0xfadb60;   // 0.5f
inline constexpr std::uint64_t kSlot518TailMulScaleRva = 0xfadb64;  // 0.866f
inline constexpr std::uint64_t kSlot518TailOneRva = 0xfadb68;       // 1.0f
inline constexpr std::uint32_t kSlot518TailMulHalfBits = 0x3f000000u;
inline constexpr std::uint32_t kSlot518TailMulScaleBits = 0x3f5db22du;
inline constexpr std::uint32_t kSlot518TailOneBits = 0x3f800000u;
inline constexpr std::uint32_t kSlot518CacheDefaultBits = 0x41080000u;   // 8.5f
inline constexpr std::uint32_t kSlot518TailE0Bits = 0x40880000u;         // 8.5f * 0.5f = 4.25f
inline constexpr std::uint32_t kSlot518TailE4Bits = 0x40eb8d50u;         // (4.25f + 4.25f) * 0.866f = 7.361f
inline constexpr std::uint32_t kSlot518TailE8Bits = 0x3e0b1c8du;         // 1.0f / E4 = 0.13585f

// The eleven tail dwords at their original offsets from 0x2ee66c0, in address order.
struct AmbientSpacingBasis {
    std::uint32_t pair0[2];   // 0x2ee66c0
    std::uint32_t pair1[2];   // 0x2ee66c8
    std::uint32_t pair2[2];   // 0x2ee66d0
    std::uint32_t pair3[2];   // 0x2ee66d8
    std::uint32_t spacing;    // 0x2ee66e0  cache * 0.5f
    std::uint32_t pitch;      // 0x2ee66e4  (spacing + spacing) * 0.866f
    std::uint32_t inverse;    // 0x2ee66e8  1.0f / pitch
};
static_assert(sizeof(AmbientSpacingBasis) == 0x2c && offsetof(AmbientSpacingBasis, spacing) == 0x20 &&
                  offsetof(AmbientSpacingBasis, inverse) == 0x28,
              "the tail cells keep the original offsets 0x2ee66c0..0x2ee66ec");

// Native stand-ins: the variable at its stride, then the tail cells that follow it in .bss.
struct Slot518Objects {
    ConCmdRangedVar var{};          // 0x2ee65e8, 0xd8
    AmbientSpacingBasis basis{};    // 0x2ee66c0, 0x2c
    std::uint32_t cache_bits = kSlot518CacheDefaultBits;   // *0x16048a0 (file-backed .data)
};
static_assert(sizeof(ConCmdRangedVar) == debug_page::kVarStride, "the stand-in must keep the original stride");
static_assert(offsetof(Slot518Objects, basis) == debug_page::kVarStride,
              "the tail cells follow the variable exactly as 0x2ee65e8 + 0xd8 = 0x2ee66c0");
static_assert(kSlot518VarRva + debug_page::kVarStride == kSlot518TailBaseRva);

// The tail alone: writes the eleven dwords from the cache bits. Never fails; the IEEE single results are the
// ones the original's vmulss / vaddss / vdivss produce.
void apply_ambient_spacing_tail(AmbientSpacingBasis& basis, std::uint32_t cache_bits) noexcept;

inline constexpr std::array<debug_page::FloatVarRow, 1> kSlot518Vars = {{
    {kSlot518VarRva, kSlot518VarName, 0x15, kSlot518VarDescription, 0x00, kSlot518VarLoBits,
     kSlot518VarHiBits, kSlot518VarValueBits, debug_page::kArgFnVtblSlot518, 0x1a58c1, 0x1a58dd, 0x1a58f7,
     0x1a5915},
}};
inline constexpr debug_page::Script kSlot518Script = {
    kSlot518Rva,          kSlot518BodySize,   kSlot518InstructionCount, kSlot518DirectCallSites,
    kSlot518FunctorSites, kSlot518TableIndex, kSlot518CanaryFrameOffset, kSlot518BranchRva,
    kSlot518BranchTargetRva,
    std::span<const debug_page::FloatVarRow>(kSlot518Vars),
    std::span<const debug_page::MenuRow>(),
    std::span<const debug_page::HashedNameRow>()};

// Result of the fragment: the shared page result plus whether the tail ran.
struct Slot518Result {
    debug_page::Result page{};
    bool tail_written = false;   // true iff the variable block completed (its atexit record exists)
    bool ok = false;             // page.ok && tail_written
};

[[nodiscard]] Slot518Result init_slot518_1a57d0(Slot518Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const debug_page::Hooks& hooks = {}) noexcept;

}  // namespace sotc
