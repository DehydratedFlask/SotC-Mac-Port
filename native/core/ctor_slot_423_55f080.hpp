// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): native model of startup constructor slot 423 (RVA 0x55f080, 1000
// bytes). Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 423. Bool debug-page
// shape: 4 BOOL console variable(s) in the order of the shared init_bool_var_static sequence (0x20af0, mid vptr,
// field stores, caller functor tag 0x15ca5a8, f590(description), final vptr, value/default byte, 0x20d60, f590(name),
// __cxa_atexit(0x20b0, var)), then 2 hashed-name record(s).
// Every body fact is byte-pinned by tools/lift/check_ctor_slot_423.py. NOTHING here executes, loads or calls an
// original RVA: the vtables, the functor tag and the destructor RVAs are IDENTITY TAGS, recorded and never run.
// Names and descriptions are stack views (control == nullptr), so the 0xf590 releases are exact no-ops. The
// __stack_chk_guard compare is a hook (not modelled), exactly as in every other lane-M6 bool slot.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot423Rva = 0x55f080;
inline constexpr std::size_t kSlot423Size = 1000;
inline constexpr std::uint64_t kSlot423BoolFnTag = 0x15ca5a8;
inline constexpr char kSlot423Var0Name[] = "forceAllowNewGamePlus";
inline constexpr std::size_t kSlot423Var0NameLength = 0x15;
inline constexpr char kSlot423Var1Name[] = "shadowDemoMode_UnlockAllColossi";
inline constexpr std::size_t kSlot423Var1NameLength = 0x1f;
inline constexpr char kSlot423Var2Name[] = "shadowDemoMode_UnlockPerformanceMode";
inline constexpr std::size_t kSlot423Var2NameLength = 0x24;
inline constexpr char kSlot423Var3Name[] = "shadowDemoMode_gameplayIntro";
inline constexpr std::size_t kSlot423Var3NameLength = 0x1c;
inline constexpr char kSlot423Record0Name[] = "SecretRoomCollectibleCount";
inline constexpr std::uint32_t kSlot423Record0Hash = 0x030d5c4f;
inline constexpr char kSlot423Record1Name[] = "JustSelectedItem";
inline constexpr std::uint32_t kSlot423Record1Hash = 0xf17781ad;

struct Slot423Objects {
    ConCmdBoolVar var0;   // 0x2f06068
    ConCmdBoolVar var1;   // 0x2f06130
    ConCmdBoolVar var2;   // 0x2f061f8
    ConCmdBoolVar var3;   // 0x2f062c0
    NameHashCell record0;   // 0x2f06388
    NameHashCell record1;   // 0x2f06398
};

struct Slot423Result {
    bool ok = false;
    BoolVarResult var0{};
    BoolVarResult var1{};
    BoolVarResult var2{};
    BoolVarResult var3{};
    std::size_t records_written = 0;   // 2 `{name, fnv1a(name)}` record(s), written after the variables
};

[[nodiscard]] Slot423Result init_slot423_55f080(Slot423Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
