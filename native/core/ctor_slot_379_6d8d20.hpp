// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: startup constructor index 379 / RVA 0x6d8d20 (FDE 512 B
// [0x6d8d20, 0x6d8f20), 105 instructions), ELF sha256 d0723afb...5861d3. Offline analysis only.
//
// A DATA row of core/debug_page_var_item: one bounded float console variable plus one 0x26ea0 toggle
// item — the same statement sequence as slots 361 / 467 with the toggle constructor instead of the
// slider. Every fact below is pinned by tools/lift/check_ctor_slot_379.py.
//
// Characterisation (from the pinned bytes):
//   * the startup table word at 0x2dd70e0 - 379*8 = 0x2dd6508 holds 0x6d8d20;
//   * the same frame/guard, exactly one conditional branch (0x6d8f09 `jne` -> 0x6d8f1a) and exactly one
//     indirect call (0x6d8e31 `callq *0x18(%rax)`);
//   * 10 direct call sites over 6 callees: 0xc300 x1, 0xf590 x4, 0x20d60 x1, 0x26ea0 x1,
//     __cxa_atexit x2, __stack_chk_fail x1;
//   * the variable `shadow_buddhaBossPlayerOutsideAttackRadius` at 0x323be40 (0xd8 B), value cell = its
//     own +0xd0, range [0.0, 100.0], value = default = 15.0; the toggle at 0x323bf18 (= var + 0xd8,
//     0xa8 B), label "Shadow AI", text "Buddha Boss Player Outside Attack Radius", flags 0x30;
//   * the caller argument-functor tag 0x15cc898 (its four .data.rel.ro slots are the float bodies
//     0xc4a0/0xc4c0/0xc4e0/0xc4f0);
//   * two __cxa_atexit records in this order: (0x6d8e6e, dtor 0x20f0, var 0x323be40) and
//     (0x6d8ef2, dtor 0x272d0, item 0x323bf18), both with __dso_handle 0x1604218.
//
// 0x26ea0 reads two virtual slots of its target. The target here is the float variable class
// 0x15942b0: relocation-pinned slot 3 = 0xbba0 (`mov eax,1; ret`) and slot 6 = 0xbbf0
// (`mov eax,3; ret`), so `slot3 == 1 && slot6 == 3` HOLDS and the item range narrows from the
// constructor default {1.0, 10.0} to {0.1, 0.5}. That answer is data of the model (the class tag),
// never a hook, and the checker pins both relocated bodies.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

#include "debug_page_var_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot379Rva = 0x6d8d20;
inline constexpr std::size_t kSlot379BodySize = 512;               // FDE [0x6d8d20, 0x6d8f20)
inline constexpr std::uint64_t kSlot379EndRva = 0x6d8f20;
inline constexpr std::size_t kSlot379InstructionCount = 105;
inline constexpr std::size_t kSlot379DirectCallSites = 10;
inline constexpr std::size_t kSlot379DirectCallees = 6;
inline constexpr std::size_t kSlot379FunctorSites = 1;
inline constexpr std::size_t kSlot379BranchCount = 1;
inline constexpr std::uint64_t kSlot379TableTopRva = 0x2dd70e0;    // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot379TableWordRva = 0x2dd6508;   // 0x2dd70e0 - 379*8
inline constexpr std::uint32_t kSlot379TableIndex = 379;

// Frame / guard.
inline constexpr std::uint64_t kSlot379CanaryCellRva = 0x15fe518;
inline constexpr std::uint32_t kSlot379CanaryFrameOffset = 0xc0;
inline constexpr std::uint64_t kSlot379BranchRva = 0x6d8f09;       // jne 0x6d8f1a (the only branch)
inline constexpr std::uint64_t kSlot379BranchTargetRva = 0x6d8f1a; // callq __stack_chk_fail

// Objects (identity addresses; the stand-ins live in Slot379Objects at the same strides).
inline constexpr std::uint64_t kSlot379VarRva = 0x323be40;
inline constexpr std::uint64_t kSlot379ItemRva = 0x323bf18;        // var + 0xd8

// Strings: identity address + pinned length.
inline constexpr std::uint64_t kSlot379VarNameRva = 0x12761b8;
inline constexpr std::uint64_t kSlot379VarDescriptionRva = 0x1276298;   // empty
inline constexpr std::uint64_t kSlot379ItemLabelRva = 0x12761e3;
inline constexpr std::uint64_t kSlot379ItemTextRva = 0x12761ed;
inline constexpr char kSlot379VarName[] = "shadow_buddhaBossPlayerOutsideAttackRadius";   // 0x2a
inline constexpr char kSlot379VarDescription[] = "";                                      // length 0
inline constexpr char kSlot379ItemLabel[] = "Shadow AI";                                  // 9
inline constexpr char kSlot379ItemText[] = "Buddha Boss Player Outside Attack Radius";    // 0x28
static_assert(sizeof kSlot379VarName - 1 == 0x2a && sizeof kSlot379VarDescription - 1 == 0x00);
static_assert(sizeof kSlot379ItemLabel - 1 == 0x09 && sizeof kSlot379ItemText - 1 == 0x28);

// Range and value of the variable.
inline constexpr std::uint32_t kSlot379VarLoBits = 0x00000000u;    // 0.0f
inline constexpr std::uint32_t kSlot379VarHiBits = 0x42c80000u;    // 100.0f
inline constexpr std::uint32_t kSlot379VarValueBits = 0x41700000u; // 15.0f
// The toggle's range after the gate answer {slot3 = 1, slot6 = 3} (0x26ea0's own narrowing).
inline constexpr std::uint32_t kSlot379ToggleLoBits = 0x3dcccccd;  // 0.1f
inline constexpr std::uint32_t kSlot379ToggleHiBits = 0x3f000000;  // 0.5f

struct Slot379Objects {
    ConCmdRangedVar var{};    // 0x323be40, 0xd8
    DebugMenuItem item{};     // 0x323bf18, 0xa8
};
static_assert(sizeof(ConCmdRangedVar) == debug_page::kVarStride && sizeof(DebugMenuItem) == debug_page::kItemStride,
              "the stand-ins must keep the original strides");
static_assert(offsetof(Slot379Objects, item) == debug_page::kVarStride && sizeof(Slot379Objects) == 0x180);
static_assert(kSlot379VarRva + debug_page::kVarStride == kSlot379ItemRva,
              "the original object addresses follow the same strides");

inline constexpr std::array<debug_page::FloatVarRow, 1> kSlot379Vars = {{
    {kSlot379VarRva, kSlot379VarName, 0x2a, kSlot379VarDescription, 0x00, kSlot379VarLoBits,
     kSlot379VarHiBits, kSlot379VarValueBits, debug_page::kArgFnVtblSlot379, 0x6d8e15, 0x6d8e31, 0x6d8e4d,
     0x6d8e6e},
}};
inline constexpr std::array<debug_page::MenuRow, 1> kSlot379Items = {{
    {debug_page::MenuKind::toggle_26ea0, kSlot379ItemRva, 0, kSlot379ItemLabel, 0x09, kSlot379ItemText, 0x28,
     0.0f, 0.0f, 0x6d8ed0, 0x6d8ef2},   // the range words are unused for a toggle (see the header)
}};
inline constexpr debug_page::Script kSlot379Script = {
    kSlot379Rva,          kSlot379BodySize,   kSlot379InstructionCount, kSlot379DirectCallSites,
    kSlot379FunctorSites, kSlot379TableIndex, kSlot379CanaryFrameOffset, kSlot379BranchRva,
    kSlot379BranchTargetRva,
    std::span<const debug_page::FloatVarRow>(kSlot379Vars),
    std::span<const debug_page::MenuRow>(kSlot379Items),
    std::span<const debug_page::HashedNameRow>()};  // no leading {name, hash} record (slot 374 only)

[[nodiscard]] debug_page::Result init_slot379_6d8d20(Slot379Objects& objects, ConCmdRegistry& concmd,
                                                     DebugMenuRegistry& menu,
                                                     const debug_page::Hooks& hooks = {}) noexcept;

}  // namespace sotc
