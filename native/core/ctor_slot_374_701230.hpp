// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: startup constructor index 374 / RVA 0x701230 (FDE 537 B
// [0x701230, 0x701449), 110 instructions), ELF sha256 d0723afb...5861d3. Offline analysis only.
//
// A DATA row of core/debug_page_var_item: the batch's one-variable + 0x26ea0-toggle shape, preceded by
// ONE `{name, fnv1a(name)}` record. Every fact below is pinned by tools/lift/check_ctor_slot_374.py.
//
// Characterisation (from the pinned bytes):
//   * the startup table word at 0x2dd70e0 - 374*8 = 0x2dd6530 holds 0x701230;
//   * prologue `push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0xe0`, guard
//     `*[0x15fe518]` at 0xc0(%rsp), exactly one conditional branch (0x701432 `jne` -> 0x701443, the
//     __stack_chk_fail call), exactly one indirect call (0x70135a `callq *0x18(%rax)`);
//   * 11 direct call sites over 7 callees: 0x52a990 x1, 0xc300 x1, 0xf590 x4, 0x20d60 x1, 0x26ea0 x1,
//     __cxa_atexit x2, __stack_chk_fail x1;
//   * the leading record at 0x3244978 (0x10 B, the NameHashCell shape): `[0x3244978] = "IsPraying"`
//     then `[0x3244980] = 0x52a990("IsPraying") = 0x411fa39f` (`mov esi,0x811c9dc5; jmp 0x161fb0`, the
//     FNV-1a already native in core/fnv_cstr_161fb0);
//   * the variable `shadow_prayTime` at 0x3244988 (= record + 0x10, 0xd8 B), value cell = its own
//     +0xd0, range [0.0, 10.0], value = default = 2.0; the toggle at 0x3244a60 (= var + 0xd8, 0xa8 B),
//     label "Save System", text "Shadow Prayer Time (s)", flags 0x30;
//   * the caller argument-functor tag 0x15cccf0 (its four .data.rel.ro slots are the float bodies
//     0xc4a0/0xc4c0/0xc4e0/0xc4f0, so the site's `callq *0x18(%rax)` is the bare-ret destroy);
//   * two __cxa_atexit records in this order: (0x701397, dtor 0x20f0, var 0x3244988) and
//     (0x70141b, dtor 0x272d0, item 0x3244a60), both with __dso_handle 0x1604218.
//
// 0x26ea0 reads two virtual slots of its target. The target here is the float variable class
// 0x15942b0: relocation-pinned slot 3 = 0xbba0 (`mov eax,1; ret`) and slot 6 = 0xbbf0
// (`mov eax,3; ret`), so `slot3 == 1 && slot6 == 3` HOLDS and the toggle range narrows from the
// constructor default {1.0, 10.0} to {0.1, 0.5}.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

#include "debug_page_var_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot374Rva = 0x701230;
inline constexpr std::size_t kSlot374BodySize = 537;               // FDE [0x701230, 0x701449)
inline constexpr std::uint64_t kSlot374EndRva = 0x701449;
inline constexpr std::size_t kSlot374InstructionCount = 110;
inline constexpr std::size_t kSlot374DirectCallSites = 11;
inline constexpr std::size_t kSlot374DirectCallees = 7;
inline constexpr std::size_t kSlot374FunctorSites = 1;
inline constexpr std::size_t kSlot374BranchCount = 1;
inline constexpr std::uint64_t kSlot374TableTopRva = 0x2dd70e0;    // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot374TableWordRva = 0x2dd6530;   // 0x2dd70e0 - 374*8
inline constexpr std::uint32_t kSlot374TableIndex = 374;

// Frame / guard.
inline constexpr std::uint64_t kSlot374CanaryCellRva = 0x15fe518;
inline constexpr std::uint32_t kSlot374CanaryFrameOffset = 0xc0;
inline constexpr std::uint64_t kSlot374BranchRva = 0x701432;       // jne 0x701443 (the only branch)
inline constexpr std::uint64_t kSlot374BranchTargetRva = 0x701443; // callq __stack_chk_fail

// Objects (identity addresses; the stand-ins live in Slot374Objects at the same strides).
inline constexpr std::uint64_t kSlot374RecordRva = 0x3244978;      // the {name, hash} cell
inline constexpr std::uint64_t kSlot374VarRva = 0x3244988;         // record + 0x10
inline constexpr std::uint64_t kSlot374ItemRva = 0x3244a60;        // var + 0xd8

// Strings: identity address + pinned length (the native views are literals).
inline constexpr std::uint64_t kSlot374RecordNameRva = 0x1278c40;
inline constexpr std::uint64_t kSlot374VarNameRva = 0x1278c4a;
inline constexpr std::uint64_t kSlot374VarDescriptionRva = 0x1278ce5;   // empty
inline constexpr std::uint64_t kSlot374ItemLabelRva = 0x1278c5a;
inline constexpr std::uint64_t kSlot374ItemTextRva = 0x1278c66;
inline constexpr char kSlot374RecordName[] = "IsPraying";                        // 0x09
inline constexpr char kSlot374VarName[] = "shadow_prayTime";                     // 0x0f
inline constexpr char kSlot374VarDescription[] = "";                             // length 0
inline constexpr char kSlot374ItemLabel[] = "Save System";                       // 0x0b
inline constexpr char kSlot374ItemText[] = "Shadow Prayer Time (s)";             // 0x16
static_assert(sizeof kSlot374RecordName - 1 == 0x09 && sizeof kSlot374VarName - 1 == 0x0f);
static_assert(sizeof kSlot374VarDescription - 1 == 0x00);
static_assert(sizeof kSlot374ItemLabel - 1 == 0x0b && sizeof kSlot374ItemText - 1 == 0x16);

// The record's hash: 0x52a990("IsPraying") = FNV-1a/32 with the 0x811c9dc5 basis (sign-extended bytes;
// ASCII here, so identical to a byte-wise unsigned walk). Pinned by the checker, recomputed by the model.
inline constexpr std::uint32_t kSlot374RecordHash = 0x411fa39fu;

// Range and value of the variable.
inline constexpr std::uint32_t kSlot374VarLoBits = 0x00000000u;    // 0.0f
inline constexpr std::uint32_t kSlot374VarHiBits = 0x41200000u;    // 10.0f
inline constexpr std::uint32_t kSlot374VarValueBits = 0x40000000u; // 2.0f
// The toggle's range after the gate answer {slot3 = 1, slot6 = 3} (0x26ea0's own narrowing).
inline constexpr std::uint32_t kSlot374ToggleLoBits = 0x3dcccccd;  // 0.1f
inline constexpr std::uint32_t kSlot374ToggleHiBits = 0x3f000000;  // 0.5f

// Native stand-ins: the original strides, in the original order.
struct Slot374Objects {
    NameHashCell record{};    // 0x3244978, 0x10
    ConCmdRangedVar var{};    // 0x3244988, 0xd8
    DebugMenuItem item{};     // 0x3244a60, 0xa8
};
static_assert(sizeof(NameHashCell) == 0x10 && offsetof(Slot374Objects, var) == 0x10);
static_assert(offsetof(Slot374Objects, item) == 0x10 + debug_page::kVarStride &&
              sizeof(Slot374Objects) == 0x10 + debug_page::kVarStride + debug_page::kItemStride);
static_assert(sizeof(ConCmdRangedVar) == debug_page::kVarStride && sizeof(DebugMenuItem) == debug_page::kItemStride,
              "the stand-ins must keep the original strides");
static_assert(kSlot374RecordRva + 0x10 == kSlot374VarRva &&
                  kSlot374VarRva + debug_page::kVarStride == kSlot374ItemRva,
              "the original object addresses follow the same strides");

inline constexpr std::array<debug_page::HashedNameRow, 1> kSlot374HashedNames = {{
    {kSlot374RecordRva, kSlot374RecordName, 0x09, 0x701268, 0x70126b},
}};
inline constexpr std::array<debug_page::FloatVarRow, 1> kSlot374Vars = {{
    {kSlot374VarRva, kSlot374VarName, 0x0f, kSlot374VarDescription, 0x00, kSlot374VarLoBits,
     kSlot374VarHiBits, kSlot374VarValueBits, debug_page::kArgFnVtblSlot374, 0x70133e, 0x70135a, 0x701376,
     0x701397},
}};
inline constexpr std::array<debug_page::MenuRow, 1> kSlot374Items = {{
    {debug_page::MenuKind::toggle_26ea0, kSlot374ItemRva, 0, kSlot374ItemLabel, 0x0b, kSlot374ItemText, 0x16,
     0.0f, 0.0f, 0x7013f9, 0x70141b},   // the range words are unused for a toggle (see the header)
}};
inline constexpr debug_page::Script kSlot374Script = {
    kSlot374Rva,          kSlot374BodySize,   kSlot374InstructionCount, kSlot374DirectCallSites,
    kSlot374FunctorSites, kSlot374TableIndex, kSlot374CanaryFrameOffset, kSlot374BranchRva,
    kSlot374BranchTargetRva,
    std::span<const debug_page::FloatVarRow>(kSlot374Vars),
    std::span<const debug_page::MenuRow>(kSlot374Items),
    std::span<const debug_page::HashedNameRow>(kSlot374HashedNames)};

[[nodiscard]] debug_page::Result init_slot374_701230(Slot374Objects& objects, ConCmdRegistry& concmd,
                                                     DebugMenuRegistry& menu,
                                                     const debug_page::Hooks& hooks = {}) noexcept;

}  // namespace sotc
