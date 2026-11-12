// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): native model of startup constructor slot 362 (RVA 0x76b8d0, 760
// bytes). Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 362. Bool debug-page
// shape: 3 BOOL console variable(s) in the order of the shared init_bool_var_static sequence (0x20af0, mid vptr,
// field stores, caller functor tag 0x15cd9f8, f590(description), final vptr, value/default byte, 0x20d60, f590(name),
// __cxa_atexit(0x20b0, var)), then 1 hashed-name record(s).
// Every body fact is byte-pinned by tools/lift/check_ctor_slot_362.py. NOTHING here executes, loads or calls an
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

inline constexpr std::uint64_t kSlot362Rva = 0x76b8d0;
inline constexpr std::size_t kSlot362Size = 760;
inline constexpr std::uint64_t kSlot362BoolFnTag = 0x15cd9f8;
inline constexpr char kSlot362Var0Name[] = "collision_showLizards";
inline constexpr std::size_t kSlot362Var0NameLength = 0x15;
inline constexpr char kSlot362Var1Name[] = "shadow_invincibleLizards";
inline constexpr std::size_t kSlot362Var1NameLength = 0x18;
inline constexpr char kSlot362Var2Name[] = "cp3_animalShowLizardSpawnFailure";
inline constexpr std::size_t kSlot362Var2NameLength = 0x20;
inline constexpr char kSlot362Record0Name[] = "LizardKillWithSpecialBow";
inline constexpr std::uint32_t kSlot362Record0Hash = 0xcfb05cf0;

struct Slot362Objects {
    ConCmdBoolVar var0;   // 0x3253610
    ConCmdBoolVar var1;   // 0x32536d8
    ConCmdBoolVar var2;   // 0x32537a0
    NameHashCell record0;   // 0x3253868
};

struct Slot362Result {
    bool ok = false;
    BoolVarResult var0{};
    BoolVarResult var1{};
    BoolVarResult var2{};
    std::size_t records_written = 0;   // 1 `{name, fnv1a(name)}` record(s), written after the variables
};

[[nodiscard]] Slot362Result init_slot362_76b8d0(Slot362Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
