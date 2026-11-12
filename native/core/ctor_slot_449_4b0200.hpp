// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): native model of startup constructor slot 449 (RVA 0x4b0200, 776
// bytes). Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 449. Bool debug-page
// shape: 2 BOOL console variable(s) in the order of the shared init_bool_var_static sequence (0x20af0, mid vptr,
// field stores, caller functor tag 0x15c0798, f590(description), final vptr, value/default byte, 0x20d60, f590(name),
// __cxa_atexit(0x20b0, var)), 2 0x26ea0 toggle item(s) on the bool variable(s).
// Every body fact is byte-pinned by tools/lift/check_ctor_slot_449.py. NOTHING here executes, loads or calls an
// original RVA: the vtables, the functor tag and the destructor RVAs are IDENTITY TAGS, recorded and never run.
// Names and descriptions are stack views (control == nullptr), so the 0xf590 releases are exact no-ops. The
// __stack_chk_guard compare is a hook (not modelled), exactly as in every other lane-M6 bool slot.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot449Rva = 0x4b0200;
inline constexpr std::size_t kSlot449Size = 776;
inline constexpr std::uint64_t kSlot449BoolFnTag = 0x15c0798;
inline constexpr char kSlot449Var0Name[] = "showActiveCutscenes";
inline constexpr std::size_t kSlot449Var0NameLength = 0x13;
inline constexpr char kSlot449Item0Label[] = "Cutscene";
inline constexpr std::size_t kSlot449Item0LabelLength = 0x8;
inline constexpr char kSlot449Item0Text[] = "Show Active Cutscenes";
inline constexpr std::size_t kSlot449Item0TextLength = 0x15;
inline constexpr char kSlot449Var1Name[] = "showCutsceneEvents";
inline constexpr std::size_t kSlot449Var1NameLength = 0x12;
inline constexpr char kSlot449Item1Label[] = "Cutscene";
inline constexpr std::size_t kSlot449Item1LabelLength = 0x8;
inline constexpr char kSlot449Item1Text[] = "Show Events";
inline constexpr std::size_t kSlot449Item1TextLength = 0xb;

struct Slot449Objects {
    ConCmdBoolVar var0;   // 0x2efa400
    DebugMenuItem item0;   // 0x2efa4c8, target = var0
    ConCmdBoolVar var1;   // 0x2efa570
    DebugMenuItem item1;   // 0x2efa638, target = var1
};

struct Slot449Result {
    bool ok = false;
    BoolVarResult var0{};
    MenuSiteResult item0{};
    BoolVarResult var1{};
    MenuSiteResult item1{};
};

[[nodiscard]] Slot449Result init_slot449_4b0200(Slot449Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
