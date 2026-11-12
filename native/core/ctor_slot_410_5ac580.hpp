// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): native model of startup constructor slot 410 (RVA 0x5ac580, 735
// bytes). Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 410. Bool debug-page
// shape: 3 BOOL console variable(s) in the order of the shared init_bool_var_static sequence (0x20af0, mid vptr,
// field stores, caller functor tag 0x15cb188, f590(description), final vptr, value/default byte, 0x20d60, f590(name),
// __cxa_atexit(0x20b0, var)).
// Every body fact is byte-pinned by tools/lift/check_ctor_slot_410.py. NOTHING here executes, loads or calls an
// original RVA: the vtables, the functor tag and the destructor RVAs are IDENTITY TAGS, recorded and never run.
// Names and descriptions are stack views (control == nullptr), so the 0xf590 releases are exact no-ops. The
// __stack_chk_guard compare is a hook (not modelled), exactly as in every other lane-M6 bool slot.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot410Rva = 0x5ac580;
inline constexpr std::size_t kSlot410Size = 735;
inline constexpr std::uint64_t kSlot410BoolFnTag = 0x15cb188;
inline constexpr char kSlot410Var0Name[] = "collision_useBIH";
inline constexpr std::size_t kSlot410Var0NameLength = 0x10;
inline constexpr char kSlot410Var1Name[] = "collision_showPlayer";
inline constexpr std::size_t kSlot410Var1NameLength = 0x14;
inline constexpr char kSlot410Var2Name[] = "collision_drawEnableZ";
inline constexpr std::size_t kSlot410Var2NameLength = 0x15;

struct Slot410Objects {
    ConCmdBoolVar var0;   // 0x2fd9fa0
    ConCmdBoolVar var1;   // 0x2fda068
    ConCmdBoolVar var2;   // 0x2fda130
};

struct Slot410Result {
    bool ok = false;
    BoolVarResult var0{};
    BoolVarResult var1{};
    BoolVarResult var2{};
};

[[nodiscard]] Slot410Result init_slot410_5ac580(Slot410Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
