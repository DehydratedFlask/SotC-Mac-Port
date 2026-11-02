// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: startup constructor index 361 / RVA 0x777390 (FDE 528 B
// [0x777390, 0x7775a0), 107 instructions), ELF sha256 d0723afb...5861d3. Offline analysis only.
//
// This is a DATA row of the batch model core/debug_page_var_item: one bounded float console variable
// plus one 0x27050 slider item, i.e. the exact statement sequence documented in that header. Nothing
// about the body's shape is re-implemented here; the per-slot facts are these constants and the two
// row tables below, and every one of them is pinned by tools/lift/check_ctor_slot_361.py.
//
// Characterisation (from the pinned bytes):
//   * reached by DATA only: the startup table word at 0x2dd70e0 - 361*8 = 0x2dd6598 holds 0x777390;
//   * prologue `push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0xe0`, guard
//     `*[0x15fe518]` at 0xc0(%rsp), exactly one conditional branch (0x777589 `jne` -> 0x77759a, the
//     __stack_chk_fail call), exactly one indirect call (0x7774a1 `callq *0x18(%rax)`);
//   * 10 direct call sites over 6 callees: 0xc300 x1, 0xf590 x4, 0x20d60 x1, 0x27050 x1,
//     __cxa_atexit x2, __stack_chk_fail x1;
//   * the variable `shadow_snakeBossDiveLoopLength` at 0x3254fb0 (0xd8 B), value cell = its own +0xd0,
//     range [0.0, 30.0], value = default = 14.0; the slider at 0x3255088 (= var + 0xd8, 0xa8 B),
//     label "Shadow AI", text "Snake Boss Dive Loop Audio Length", range [0.1, 1.0], flags 0x30;
//   * the caller argument-functor tag 0x15cda18 (its four .data.rel.ro slots are the float bodies
//     0xc4a0/0xc4c0/0xc4e0/0xc4f0, so the site's `callq *0x18(%rax)` is the bare-ret destroy);
//   * two __cxa_atexit records in this order: (0x7774de, dtor 0x20f0, var 0x3254fb0) and
//     (0x777572, dtor 0x272d0, item 0x3255088), both with __dso_handle 0x1604218.
//
// The 0x27050 slider passes its range explicitly and reads NO virtual slot of the target; the target
// pointer the item stores (+0x90) is the variable object itself.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

#include "debug_page_var_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot361Rva = 0x777390;
inline constexpr std::size_t kSlot361BodySize = 528;               // FDE [0x777390, 0x7775a0)
inline constexpr std::uint64_t kSlot361EndRva = 0x7775a0;
inline constexpr std::size_t kSlot361InstructionCount = 107;
inline constexpr std::size_t kSlot361DirectCallSites = 10;
inline constexpr std::size_t kSlot361DirectCallees = 6;
inline constexpr std::size_t kSlot361FunctorSites = 1;
inline constexpr std::size_t kSlot361BranchCount = 1;
inline constexpr std::uint64_t kSlot361TableTopRva = 0x2dd70e0;    // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot361TableWordRva = 0x2dd6598;   // 0x2dd70e0 - 361*8
inline constexpr std::uint32_t kSlot361TableIndex = 361;

// Frame / guard.
inline constexpr std::uint64_t kSlot361CanaryCellRva = 0x15fe518;  // GOT cell of __stack_chk_guard
inline constexpr std::uint32_t kSlot361CanaryFrameOffset = 0xc0;
inline constexpr std::uint64_t kSlot361BranchRva = 0x777589;       // jne 0x77759a (the only branch)
inline constexpr std::uint64_t kSlot361BranchTargetRva = 0x77759a; // callq __stack_chk_fail

// Objects (identity addresses; the stand-ins live in Slot361Objects at the same strides).
inline constexpr std::uint64_t kSlot361VarRva = 0x3254fb0;
inline constexpr std::uint64_t kSlot361ItemRva = 0x3255088;        // var + 0xd8

// Strings: identity address + pinned length (the native views are literals).
inline constexpr std::uint64_t kSlot361VarNameRva = 0x1282568;
inline constexpr std::uint64_t kSlot361VarDescriptionRva = 0x1282696;   // empty
inline constexpr std::uint64_t kSlot361ItemLabelRva = 0x1282587;
inline constexpr std::uint64_t kSlot361ItemTextRva = 0x1282591;
inline constexpr char kSlot361VarName[] = "shadow_snakeBossDiveLoopLength";        // 0x1e
inline constexpr char kSlot361VarDescription[] = "";                             // length 0
inline constexpr char kSlot361ItemLabel[] = "Shadow AI";                         // 9
inline constexpr char kSlot361ItemText[] = "Snake Boss Dive Loop Audio Length";   // 0x21
static_assert(sizeof kSlot361VarName - 1 == 0x1e && sizeof kSlot361VarDescription - 1 == 0x00);
static_assert(sizeof kSlot361ItemLabel - 1 == 0x09 && sizeof kSlot361ItemText - 1 == 0x21);

// Range, value and the slider's range words (the site loads both from rodata: 0x1282560 / 0x1282564).
inline constexpr std::uint32_t kSlot361VarLoBits = 0x00000000u;   // 0.0f
inline constexpr std::uint32_t kSlot361VarHiBits = 0x41f00000u;   // 30.0f
inline constexpr std::uint32_t kSlot361VarValueBits = 0x41600000u;  // 14.0f, stored to +0xd0 and +0x68
inline constexpr std::uint32_t kSlot361SliderLoBits = 0x3dcccccd;  // 0.1f at 0x1282560
inline constexpr std::uint32_t kSlot361SliderHiBits = 0x3f800000;  // 1.0f at 0x1282564

// Native stand-ins: the original strides, in the original order.
struct Slot361Objects {
    ConCmdRangedVar var{};    // 0x3254fb0, 0xd8
    DebugMenuItem item{};     // 0x3255088, 0xa8
};
static_assert(sizeof(ConCmdRangedVar) == debug_page::kVarStride && sizeof(DebugMenuItem) == debug_page::kItemStride,
              "the stand-ins must keep the original strides");
static_assert(offsetof(Slot361Objects, item) == debug_page::kVarStride && sizeof(Slot361Objects) == 0x180);
static_assert(kSlot361VarRva + debug_page::kVarStride == kSlot361ItemRva,
              "the original object addresses follow the same strides");

inline constexpr std::array<debug_page::FloatVarRow, 1> kSlot361Vars = {{
    {kSlot361VarRva, kSlot361VarName, 0x1e, kSlot361VarDescription, 0x00, kSlot361VarLoBits,
     kSlot361VarHiBits, kSlot361VarValueBits, debug_page::kArgFnVtblSlot361, 0x777485, 0x7774a1, 0x7774bd,
     0x7774de},
}};
inline constexpr std::array<debug_page::MenuRow, 1> kSlot361Items = {{
    {debug_page::MenuKind::slider_27050, kSlot361ItemRva, 0, kSlot361ItemLabel, 0x09, kSlot361ItemText, 0x21,
     0.1f, 1.0f, 0x777550, 0x777572},
}};
inline constexpr debug_page::Script kSlot361Script = {
    kSlot361Rva,          kSlot361BodySize,   kSlot361InstructionCount, kSlot361DirectCallSites,
    kSlot361FunctorSites, kSlot361TableIndex, kSlot361CanaryFrameOffset, kSlot361BranchRva,
    kSlot361BranchTargetRva,
    std::span<const debug_page::FloatVarRow>(kSlot361Vars),
    std::span<const debug_page::MenuRow>(kSlot361Items)};

// Runs original constructor 361 natively through the batch model. The original is unconditional, so the
// first failing step stops the fragment and `blocked` names it.
[[nodiscard]] debug_page::Result init_slot361_777390(Slot361Objects& objects, ConCmdRegistry& concmd,
                                                     DebugMenuRegistry& menu,
                                                     const debug_page::Hooks& hooks = {}) noexcept;

}  // namespace sotc
