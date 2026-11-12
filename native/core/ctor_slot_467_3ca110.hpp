// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: startup constructor index 467 / RVA 0x3ca110 (FDE 537 B
// [0x3ca110, 0x3ca329), 107 instructions), ELF sha256 d0723afb...5861d3. Offline analysis only.
//
// A DATA row of core/debug_page_var_item: one bounded float console variable plus one 0x27050 slider
// item — the same statement sequence as slot 361, with the value stores in rip-relative form instead of
// register-based form (an addressing choice, not a shape change). Every fact below is pinned by
// tools/lift/check_ctor_slot_467.py.
//
// Characterisation (from the pinned bytes):
//   * the startup table word at 0x2dd70e0 - 467*8 = 0x2dd6248 holds 0x3ca110;
//   * the same frame/guard as the rest of the batch, exactly one conditional branch (0x3ca312 `jne`
//     -> 0x3ca323, the __stack_chk_fail call) and exactly one indirect call (0x3ca225
//     `callq *0x18(%rax)`);
//   * 10 direct call sites over 6 callees: 0xc300 x1, 0xf590 x4, 0x20d60 x1, 0x27050 x1,
//     __cxa_atexit x2, __stack_chk_fail x1;
//   * the variable `consoleTextScale` at 0x2ef6e08 (0xd8 B), value cell = its own +0xd0, range
//     [0.1, 10.0], value = default = 1.0; the slider at 0x2ef6ee0 (= var + 0xd8, 0xa8 B), label
//     "Debug", text "Console Text Scale", range [0.1, 1.0], flags 0x30;
//   * the caller argument-functor tag 0x15b0e90 (its four .data.rel.ro slots are the float bodies
//     0xc4a0/0xc4c0/0xc4e0/0xc4f0);
//   * two __cxa_atexit records in this order: (0x3ca267, dtor 0x20f0, var 0x2ef6e08) and
//     (0x3ca2fb, dtor 0x272d0, item 0x2ef6ee0), both with __dso_handle 0x1604218.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

#include "debug_page_var_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot467Rva = 0x3ca110;
inline constexpr std::size_t kSlot467BodySize = 537;               // FDE [0x3ca110, 0x3ca329)
inline constexpr std::uint64_t kSlot467EndRva = 0x3ca329;
inline constexpr std::size_t kSlot467InstructionCount = 107;
inline constexpr std::size_t kSlot467DirectCallSites = 10;
inline constexpr std::size_t kSlot467DirectCallees = 6;
inline constexpr std::size_t kSlot467FunctorSites = 1;
inline constexpr std::size_t kSlot467BranchCount = 1;
inline constexpr std::uint64_t kSlot467TableTopRva = 0x2dd70e0;    // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot467TableWordRva = 0x2dd6248;   // 0x2dd70e0 - 467*8
inline constexpr std::uint32_t kSlot467TableIndex = 467;

// Frame / guard.
inline constexpr std::uint64_t kSlot467CanaryCellRva = 0x15fe518;
inline constexpr std::uint32_t kSlot467CanaryFrameOffset = 0xc0;
inline constexpr std::uint64_t kSlot467BranchRva = 0x3ca312;       // jne 0x3ca323 (the only branch)
inline constexpr std::uint64_t kSlot467BranchTargetRva = 0x3ca323; // callq __stack_chk_fail

// Objects (identity addresses; the stand-ins live in Slot467Objects at the same strides).
inline constexpr std::uint64_t kSlot467VarRva = 0x2ef6e08;
inline constexpr std::uint64_t kSlot467ItemRva = 0x2ef6ee0;        // var + 0xd8

// Strings: identity address + pinned length.
inline constexpr std::uint64_t kSlot467VarNameRva = 0xfe7bc0;
inline constexpr std::uint64_t kSlot467VarDescriptionRva = 0xfe7d50;   // empty
inline constexpr std::uint64_t kSlot467ItemLabelRva = 0xfe7bd1;
inline constexpr std::uint64_t kSlot467ItemTextRva = 0xfe7bd7;
inline constexpr char kSlot467VarName[] = "consoleTextScale";        // 0x10
inline constexpr char kSlot467VarDescription[] = "";                 // length 0
inline constexpr char kSlot467ItemLabel[] = "Debug";                 // 5
inline constexpr char kSlot467ItemText[] = "Console Text Scale";     // 0x12
static_assert(sizeof kSlot467VarName - 1 == 0x10 && sizeof kSlot467VarDescription - 1 == 0x00);
static_assert(sizeof kSlot467ItemLabel - 1 == 0x05 && sizeof kSlot467ItemText - 1 == 0x12);

// Range, value and the slider's range words (loaded from rodata 0xfe7b28 / 0xfe7b2c).
inline constexpr std::uint32_t kSlot467VarLoBits = 0x3dcccccd;      // 0.1f
inline constexpr std::uint32_t kSlot467VarHiBits = 0x41200000;      // 10.0f
inline constexpr std::uint32_t kSlot467VarValueBits = 0x3f800000;   // 1.0f
inline constexpr std::uint32_t kSlot467SliderLoBits = 0x3dcccccd;   // 0.1f at 0xfe7b28
inline constexpr std::uint32_t kSlot467SliderHiBits = 0x3f800000;   // 1.0f at 0xfe7b2c

struct Slot467Objects {
    ConCmdRangedVar var{};    // 0x2ef6e08, 0xd8
    DebugMenuItem item{};     // 0x2ef6ee0, 0xa8
};
static_assert(sizeof(ConCmdRangedVar) == debug_page::kVarStride && sizeof(DebugMenuItem) == debug_page::kItemStride,
              "the stand-ins must keep the original strides");
static_assert(offsetof(Slot467Objects, item) == debug_page::kVarStride && sizeof(Slot467Objects) == 0x180);
static_assert(kSlot467VarRva + debug_page::kVarStride == kSlot467ItemRva,
              "the original object addresses follow the same strides");

inline constexpr std::array<debug_page::FloatVarRow, 1> kSlot467Vars = {{
    {kSlot467VarRva, kSlot467VarName, 0x10, kSlot467VarDescription, 0x00, kSlot467VarLoBits,
     kSlot467VarHiBits, kSlot467VarValueBits, debug_page::kArgFnVtblSlot467, 0x3ca209, 0x3ca225, 0x3ca246,
     0x3ca267},
}};
inline constexpr std::array<debug_page::MenuRow, 1> kSlot467Items = {{
    {debug_page::MenuKind::slider_27050, kSlot467ItemRva, 0, kSlot467ItemLabel, 0x05, kSlot467ItemText, 0x12,
     0.1f, 1.0f, 0x3ca2d9, 0x3ca2fb},
}};
inline constexpr debug_page::Script kSlot467Script = {
    kSlot467Rva,          kSlot467BodySize,   kSlot467InstructionCount, kSlot467DirectCallSites,
    kSlot467FunctorSites, kSlot467TableIndex, kSlot467CanaryFrameOffset, kSlot467BranchRva,
    kSlot467BranchTargetRva,
    std::span<const debug_page::FloatVarRow>(kSlot467Vars),
    std::span<const debug_page::MenuRow>(kSlot467Items),
    std::span<const debug_page::HashedNameRow>()};  // no leading {name, hash} record (slot 374 only)

[[nodiscard]] debug_page::Result init_slot467_3ca110(Slot467Objects& objects, ConCmdRegistry& concmd,
                                                     DebugMenuRegistry& menu,
                                                     const debug_page::Hooks& hooks = {}) noexcept;

}  // namespace sotc
