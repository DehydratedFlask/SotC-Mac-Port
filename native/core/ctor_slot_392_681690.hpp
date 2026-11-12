// CPU-01-STARTUP-BATCH-7: native model of startup constructor slot 392 (RVA 0x681690, 1612 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 392. Every fact below is byte-pinned by tools/lift/check_ctor_slot_392.py and the new
// 0x27120 body by tools/lift/check_helper_27120.py. NOTHING here executes, loads or calls an original RVA: vtable pointers, the functor
// tag and the destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: four BOUNDED float console variables (0xc300, range {0.0, 2.0}, flags 1, empty description, caller functor tag
// 0x15cc618, objects 0x30127e8 + n * 0xd8, atexit 0x20f0) named `shadow_hackScale{Mammoth,Minotaur,Knight,KnightMetal}AttackEff`
// (the names are truncated in the image; the pinned lengths are the authority) with value = default {1.2, 0.5, 1.0, 1.5}; then four
// 0x27120 slider items (objects 0x3012b48 + n * 0xa8, caption `Shadow AI`, flags 0x30, range {0.01, 0.1} read from .rodata 0x1270cd4 /
// 0x1270cd8) whose TEXT is each variable's own name view and whose target is that variable. The first item registers the menu root
// (and its 0x27280 atexit record) inside 0x268b0; the later three find it initialised.
//
// NOT established: what reads the variables at run time. The "hack scale attack effect" reading from the strings is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot392Rva = 0x681690;
inline constexpr std::size_t kSlot392Size = 1612;
inline constexpr std::size_t kSlot392Count = 4;
inline constexpr std::uint64_t kSlot392FloatFnTag = 0x15cc618;
inline constexpr std::uint32_t kSlot392VarLoBits = 0x00000000;
inline constexpr std::uint32_t kSlot392VarHiBits = 0x40000000;
inline constexpr std::uint32_t kSlot392VarFlags = 1;
inline constexpr std::uint32_t kSlot392ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot392SliderLoBits = 0x3c23d70a;   // 0.01f at 0x1270cd4
inline constexpr std::uint32_t kSlot392SliderHiBits = 0x3dcccccd;   // 0.1f at 0x1270cd8
inline constexpr char kSlot392Label[] = "Shadow AI";
inline constexpr std::size_t kSlot392LabelLength = 9;
inline constexpr char kSlot392Names[kSlot392Count][40] = {
    "shadow_hackScaleMammothAttackEff", "shadow_hackScaleMinotaurAttackEff",
    "shadow_hackScaleKnightAttackEff", "shadow_hackScaleKnightMetalAttackEff"};
inline constexpr std::size_t kSlot392NameLengths[kSlot392Count] = {0x20, 0x21, 0x1f, 0x24};
inline constexpr std::uint32_t kSlot392ValueBits[kSlot392Count] = {0x3f99999a, 0x3f000000, 0x3f800000, 0x3fc00000};
inline constexpr std::uint64_t kSlot392Var0Rva = 0x30127e8;
inline constexpr std::uint64_t kSlot392Item0Rva = 0x3012b48;

struct Slot392Objects {
    ConCmdRangedVar var[kSlot392Count];     // 0x30127e8 + n * 0xd8
    DebugMenuItem item[kSlot392Count];      // 0x3012b48 + n * 0xa8
};

struct Slot392Result {
    bool ok = false;
    FloatVarResult var[kSlot392Count]{};
    MenuSiteResult item[kSlot392Count]{};
};

[[nodiscard]] Slot392Result init_slot392_681690(Slot392Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
