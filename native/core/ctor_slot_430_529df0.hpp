// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 430 (RVA 0x529df0, 539 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 430. Every body fact below is
// byte-pinned by tools/lift/check_ctor_slot_430.py. NOTHING here executes, loads or calls an original RVA: vtables,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) an 8-byte static cell at 0x2efc9e0 is zeroed, then __cxa_atexit(0x523ba0, cell) is recorded
// (0x523ba0, 46 B, loads [cell]; if non-null calls 0x525b40 and 0x170b0 and zeroes it: recorded, never run); (2) the ranged INT
// console variable `wanderForceCorruptionLevel` (object 0x2efc9e8, description `Set Corrupt Unlock.`, min -1, max
// 5, flags 1, value = default = -1, caller functor tag 0x15c8450) built by 0xc580 through the family wrapper
// helper_c580_ranged_int (the tag is a member of the 37-table int family; the wrapper returns HelperRefusal::none
// only for such a tag), call [argfn + 0x18] (bare ret), [obj] = 0x1594c08, 0x20d60, f590 x2,
// __cxa_atexit(0x18a60, var); (3) the debug-menu toggle at 0x2efcac0 (= var + 0xd8): label `Shadow Cheat`, text
// `Wander Corrupt Level`, target = the variable, flags 0x30 (0x26ea0, tail-registered through 0x268b0), f590(text),
// f590(label), __cxa_atexit(0x272d0, item). The int class answers the toggle's two virtual reads with {1, 2}, so
// the toggle keeps its default range (ranged_int_c580_menu_target).
// The __stack_chk_guard compare is a hook (not modelled). NOT established: what reads the variable at run time
// (the name suggests a corruption-unlock cheat level; INFERENCE only, no consumer is lifted).
#pragma once

#include <cstddef>
#include <cstdint>

#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot430Rva = 0x529df0;
inline constexpr std::size_t kSlot430Size = 539;
inline constexpr std::uint64_t kSlot430IntFnTag = 0x15c8450;
inline constexpr std::uint64_t kSlot430CellDtorRva = 0x523ba0;
inline constexpr char kSlot430VarName[] = "wanderForceCorruptionLevel";
inline constexpr std::size_t kSlot430VarNameLength = 0x1a;
inline constexpr char kSlot430VarDescription[] = "Set Corrupt Unlock.";
inline constexpr std::size_t kSlot430VarDescriptionLength = 0x13;
inline constexpr char kSlot430MenuLabel[] = "Shadow Cheat";
inline constexpr char kSlot430MenuText[] = "Wander Corrupt Level";
inline constexpr std::int32_t kSlot430Min = -1;
inline constexpr std::int32_t kSlot430Max = 5;
inline constexpr std::int32_t kSlot430Value = -1;
inline constexpr std::uint64_t kSlot430CellRva = 0x2efc9e0;
inline constexpr std::uint64_t kSlot430VarRva = 0x2efc9e8;
inline constexpr std::uint64_t kSlot430ItemRva = 0x2efcac0;

struct Slot430Objects {
    std::uint64_t cell;       // 0x2efc9e0
    ConCmdRangedVar var;      // 0x2efc9e8
    DebugMenuItem item;       // 0x2efcac0
};

struct Slot430Result {
    bool ok = false;
    bool cell_stored = false;
    bool cell_atexit_recorded = false;   // 0x523ba0 recorded, not run
    HelperIntSiteResult var{};
    MenuSiteResult toggle{};
};

[[nodiscard]] Slot430Result init_slot430_529df0(Slot430Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
