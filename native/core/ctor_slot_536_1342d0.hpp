// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: startup constructor index 536 / RVA 0x1342d0 (FDE 659 B
// [0x1342d0, 0x134563), 124 instructions), ELF sha256 d0723afb...5861d3. Offline analysis only.
//
// A DATA row of core/debug_page_var_item: TWO bounded float console variables and NO menu item — the
// same per-variable statement sequence as slots 361 / 467 / 379, repeated. That repetition is the whole
// difference, which is why one parameterised model serves it. Every fact below is pinned by
// tools/lift/check_ctor_slot_536.py.
//
// Characterisation (from the pinned bytes):
//   * the startup table word at 0x2dd70e0 - 536*8 = 0x2dd6020 holds 0x1342d0;
//   * the same frame/guard, exactly one conditional branch (0x13454c `jne` -> 0x13455d) and TWO
//     indirect calls (0x1343e2 / 0x1344f6, both `callq *0x18(%rax)`);
//   * 11 direct call sites over 5 callees: 0xc300 x2, 0xf590 x4, 0x20d60 x2, __cxa_atexit x2,
//     __stack_chk_fail x1;
//   * `r_probePreviewRoughness` at 0x2ee1340 (0xd8 B), range [0.0, 1.0], value = default = 0.5;
//     `r_probePreviewMetalness` at 0x2ee1418 (= first + 0xd8, 0xd8 B), range [0.0, 1.0], value =
//     default = 0.0;
//   * both sites use the same caller argument-functor tag 0x15981a0 (its four .data.rel.ro slots are the
//     float bodies 0xc4a0/0xc4c0/0xc4e0/0xc4f0);
//   * two __cxa_atexit records in this order: (0x134421, dtor 0x20f0, var1 0x2ee1340) and
//     (0x134535, dtor 0x20f0, var2 0x2ee1418), both with __dso_handle 0x1604218. No 0x272d0 record:
//     this body builds no menu item.
//   * both description views are {0xfa7855, length 0}: a data pointer with an immediate length of zero
//     (`vmovups %xmm0, 0x48(%rsp)`). The model builds the same length-0 borrowed view over a literal.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

#include "debug_page_var_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot536Rva = 0x1342d0;
inline constexpr std::size_t kSlot536BodySize = 659;               // FDE [0x1342d0, 0x134563)
inline constexpr std::uint64_t kSlot536EndRva = 0x134563;
inline constexpr std::size_t kSlot536InstructionCount = 124;
inline constexpr std::size_t kSlot536DirectCallSites = 11;
inline constexpr std::size_t kSlot536DirectCallees = 5;
inline constexpr std::size_t kSlot536FunctorSites = 2;
inline constexpr std::size_t kSlot536BranchCount = 1;
inline constexpr std::uint64_t kSlot536TableTopRva = 0x2dd70e0;    // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot536TableWordRva = 0x2dd6020;   // 0x2dd70e0 - 536*8
inline constexpr std::uint32_t kSlot536TableIndex = 536;

// Frame / guard.
inline constexpr std::uint64_t kSlot536CanaryCellRva = 0x15fe518;
inline constexpr std::uint32_t kSlot536CanaryFrameOffset = 0xc0;
inline constexpr std::uint64_t kSlot536BranchRva = 0x13454c;       // jne 0x13455d (the only branch)
inline constexpr std::uint64_t kSlot536BranchTargetRva = 0x13455d; // callq __stack_chk_fail

// Objects (identity addresses; the stand-ins live in Slot536Objects at the same strides).
inline constexpr std::uint64_t kSlot536Var1Rva = 0x2ee1340;
inline constexpr std::uint64_t kSlot536Var2Rva = 0x2ee1418;        // var1 + 0xd8

// Strings: identity address + pinned length.
inline constexpr std::uint64_t kSlot536Var1NameRva = 0xfa7820;
inline constexpr std::uint64_t kSlot536Var2NameRva = 0xfa7838;
inline constexpr std::uint64_t kSlot536DescriptionRva = 0xfa7855;  // both rows, length 0
inline constexpr char kSlot536Var1Name[] = "r_probePreviewRoughness";   // 0x17
inline constexpr char kSlot536Var2Name[] = "r_probePreviewMetalness";   // 0x17
inline constexpr char kSlot536Description[] = "";                       // length 0 at both rows
static_assert(sizeof kSlot536Var1Name - 1 == 0x17 && sizeof kSlot536Var2Name - 1 == 0x17);
static_assert(sizeof kSlot536Description - 1 == 0x00);

// Ranges and values (both sites store min 0.0, max 1.0 from immediates).
inline constexpr std::uint32_t kSlot536LoBits = 0x00000000u;        // 0.0f at both rows
inline constexpr std::uint32_t kSlot536HiBits = 0x3f800000u;        // 1.0f at both rows
inline constexpr std::uint32_t kSlot536Var1ValueBits = 0x3f000000u; // 0.5f
inline constexpr std::uint32_t kSlot536Var2ValueBits = 0x00000000u; // 0.0f

struct Slot536Objects {
    ConCmdRangedVar var1{};   // 0x2ee1340, 0xd8
    ConCmdRangedVar var2{};   // 0x2ee1418, 0xd8
};
static_assert(sizeof(ConCmdRangedVar) == debug_page::kVarStride, "the stand-in must keep the original stride");
static_assert(offsetof(Slot536Objects, var2) == debug_page::kVarStride && sizeof(Slot536Objects) == 0x1b0);
static_assert(kSlot536Var1Rva + debug_page::kVarStride == kSlot536Var2Rva,
              "the original object addresses follow the same strides");

inline constexpr std::array<debug_page::FloatVarRow, 2> kSlot536Vars = {{
    {kSlot536Var1Rva, kSlot536Var1Name, 0x17, kSlot536Description, 0x00, kSlot536LoBits, kSlot536HiBits,
     kSlot536Var1ValueBits, debug_page::kArgFnVtblSlot536, 0x1343c1, 0x1343e2, 0x134403, 0x134421},
    {kSlot536Var2Rva, kSlot536Var2Name, 0x17, kSlot536Description, 0x00, kSlot536LoBits, kSlot536HiBits,
     kSlot536Var2ValueBits, debug_page::kArgFnVtblSlot536, 0x1344da, 0x1344f6, 0x134517, 0x134535},
}};
inline constexpr debug_page::Script kSlot536Script = {
    kSlot536Rva,          kSlot536BodySize,   kSlot536InstructionCount, kSlot536DirectCallSites,
    kSlot536FunctorSites, kSlot536TableIndex, kSlot536CanaryFrameOffset, kSlot536BranchRva,
    kSlot536BranchTargetRva,
    std::span<const debug_page::FloatVarRow>(kSlot536Vars),
    std::span<const debug_page::MenuRow>(),
    std::span<const debug_page::HashedNameRow>()};  // no leading {name, hash} record (slot 374 only)

[[nodiscard]] debug_page::Result init_slot536_1342d0(Slot536Objects& objects, ConCmdRegistry& concmd,
                                                     DebugMenuRegistry& menu,
                                                     const debug_page::Hooks& hooks = {}) noexcept;

}  // namespace sotc
