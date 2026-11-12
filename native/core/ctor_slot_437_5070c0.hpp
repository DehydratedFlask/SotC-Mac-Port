// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 5): native model of startup constructor slot 437 (RVA 0x5070c0, 1142
// bytes). Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 437.
//
// Shape (every fact byte-pinned by tools/lift/check_ctor_slot_437.py):
//   * THREE bool console variables (0/0, empty description, caller functor tag 0x15c59a0) built in the shared
//     init_bool_var_static order (0x20af0 base ctor, mid vptr, field stores, tag, f590(description), final
//     vptr, value/default byte, 0x20d60, f590(name), __cxa_atexit(0x20b0, var)):
//       shared_footStepSurfaceShow / shared_footStepEventShow / shared_footLiftEventShow
//     in .bss at 0x2efc2f8 / 0x2efc3c0 / 0x2efc488 (stride 0xc8).
//   * then a 16-element array of 0x20-byte borrowed string views at 0x2efc550..0x2efc750, each filled by 0xf2f0
//     (`{ptr, strlen(ptr), 0, 0}`, the same borrowed view slot 335 models): the foot-step / foot-lift / skid-mark
//     bone-point names `bp_*`. The array is registered in FOUR chunks of four elements: after elements 0-3, 4-7,
//     8-11 and 12-15 the body calls `__cxa_atexit(chunk_dtor, NULL, &__dso_handle)` with chunk_dtor = 0x502920 /
//     0x502960 / 0x5029a0 / 0x5029e0. The destructor argument is NULL: each 63-byte destructor addresses its own
//     four cells directly and releases them with 0xf590 in the order 3, 2, 1, 0 (element 0 is a tail jump).
//
// NOTHING here executes, loads or calls an original RVA: the vtables, the functor tag and the destructor RVAs are
// IDENTITY TAGS, recorded and never run. The views are borrowed (control == nullptr), so every 0xf590 release is an
// exact no-op; `slot437_chunk_destructor` models what a chunk destructor does to those cells and is exercised by the
// fixture only (the startup registry just records the four (rva, nullptr) pairs). The __stack_chk_guard compare is a
// hook (not modelled), as in every other lane-M6 bool slot.
//
// NOT established: what reads the three bool variables or the 16 view cells at run time. The reading "bone points
// of the foot-step / skid-mark debug display" is INFERENCE from the strings; the cells have no lifted consumer.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot437Rva = 0x5070c0;
inline constexpr std::size_t kSlot437Size = 1142;
inline constexpr std::uint64_t kSlot437BoolFnTag = 0x15c59a0;
inline constexpr char kSlot437Var0Name[] = "shared_footStepSurfaceShow";
inline constexpr std::size_t kSlot437Var0NameLength = 0x1a;
inline constexpr char kSlot437Var1Name[] = "shared_footStepEventShow";
inline constexpr std::size_t kSlot437Var1NameLength = 0x18;
inline constexpr char kSlot437Var2Name[] = "shared_footLiftEventShow";
inline constexpr std::size_t kSlot437Var2NameLength = 0x18;

inline constexpr std::size_t kSlot437ViewCount = 16;
inline constexpr std::size_t kSlot437ChunkCount = 4;
inline constexpr std::size_t kSlot437ChunkSize = 4;
inline constexpr std::uint64_t kSlot437ViewArrayAddress = 0x2efc550;
inline constexpr std::uint64_t kSlot437ChunkDtorRva[kSlot437ChunkCount] = {0x502920, 0x502960, 0x5029a0, 0x5029e0};
inline constexpr const char* kSlot437ViewText[kSlot437ViewCount] = {
    "bp_front_left_foot_step", "bp_front_right_foot_step", "bp_back_left_foot_step", "bp_back_right_foot_step",
    "bp_front_left_foot_lift", "bp_front_right_foot_lift", "bp_back_left_foot_lift", "bp_back_right_foot_lift",
    "bp_skid_mark_1_start",    "bp_skid_mark_2_start",     "bp_skid_mark_3_start",   "bp_skid_mark_4_start",
    "bp_skid_mark_1_end",      "bp_skid_mark_2_end",       "bp_skid_mark_3_end",     "bp_skid_mark_4_end",
};

struct Slot437Objects {
    ConCmdBoolVar var0;   // 0x2efc2f8
    ConCmdBoolVar var1;   // 0x2efc3c0
    ConCmdBoolVar var2;   // 0x2efc488
    SharedStr views[kSlot437ViewCount];   // 0x2efc550, stride 0x20 (0xf2f0 borrowed views)
};
static_assert(sizeof(SharedStr) == 0x20);

struct Slot437Result {
    bool ok = false;
    BoolVarResult var0{};
    BoolVarResult var1{};
    BoolVarResult var2{};
    bool views_ok = false;               // all 16 views built and all four chunk records logged
    std::size_t chunk_records = 0;       // __cxa_atexit(chunk_dtor, NULL) records appended (4 on success)
};

[[nodiscard]] Slot437Result init_slot437_5070c0(Slot437Objects& objects, ConCmdRegistry& concmd) noexcept;

// What the chunk destructor `chunk` (0..3) does: 0xf590 on its four cells in the order 3, 2, 1, 0. True only if
// every release is an inert no-op (the views are borrowed). Fixture helper; never reached from the startup path.
[[nodiscard]] bool slot437_chunk_destructor(Slot437Objects& objects, std::size_t chunk) noexcept;

}  // namespace sotc
