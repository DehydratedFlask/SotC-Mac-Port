// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): native model of startup constructor slot 477 (RVA 0x3270d0, 918
// bytes). Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 477. Bool debug-page
// shape: 4 BOOL console variable(s) in the order of the shared init_bool_var_static sequence (0x20af0, mid vptr,
// field stores, caller functor tag 0x15a77b0, f590(description), final vptr, value/default byte, 0x20d60, f590(name),
// __cxa_atexit(0x20b0, var)).
// Every body fact is byte-pinned by tools/lift/check_ctor_slot_477.py. NOTHING here executes, loads or calls an
// original RVA: the vtables, the functor tag and the destructor RVAs are IDENTITY TAGS, recorded and never run.
// Names and descriptions are stack views (control == nullptr), so the 0xf590 releases are exact no-ops. The
// __stack_chk_guard compare is a hook (not modelled), exactly as in every other lane-M6 bool slot.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot477Rva = 0x3270d0;
inline constexpr std::size_t kSlot477Size = 918;
inline constexpr std::uint64_t kSlot477BoolFnTag = 0x15a77b0;
inline constexpr char kSlot477Var0Name[] = "uiMenuGridShowScissors";
inline constexpr std::size_t kSlot477Var0NameLength = 0x16;
inline constexpr char kSlot477Var0Description[] = "Disables scissor for menu grids and draws their rectangles.";
inline constexpr std::size_t kSlot477Var0DescriptionLength = 0x3b;
inline constexpr char kSlot477Var1Name[] = "uiMenuGridShowLayouts";
inline constexpr std::size_t kSlot477Var1NameLength = 0x15;
inline constexpr char kSlot477Var1Description[] = "Draws rectangles showing the extents of all children that are laid out by menu grids.";
inline constexpr std::size_t kSlot477Var1DescriptionLength = 0x55;
inline constexpr char kSlot477Var2Name[] = "uiMenuGridShowNames";
inline constexpr std::size_t kSlot477Var2NameLength = 0x13;
inline constexpr char kSlot477Var2Description[] = "Draws debug name of each menu grid at the top left of the grid.";
inline constexpr std::size_t kSlot477Var2DescriptionLength = 0x3f;
inline constexpr char kSlot477Var3Name[] = "uiMenuGridShowExtents";
inline constexpr std::size_t kSlot477Var3NameLength = 0x15;
inline constexpr char kSlot477Var3Description[] = "Draws the menu grid extents.";
inline constexpr std::size_t kSlot477Var3DescriptionLength = 0x1c;

struct Slot477Objects {
    ConCmdBoolVar var0;   // 0x2ef0568
    ConCmdBoolVar var1;   // 0x2ef0630
    ConCmdBoolVar var2;   // 0x2ef06f8
    ConCmdBoolVar var3;   // 0x2ef07c0
};

struct Slot477Result {
    bool ok = false;
    BoolVarResult var0{};
    BoolVarResult var1{};
    BoolVarResult var2{};
    BoolVarResult var3{};
};

[[nodiscard]] Slot477Result init_slot477_3270d0(Slot477Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
