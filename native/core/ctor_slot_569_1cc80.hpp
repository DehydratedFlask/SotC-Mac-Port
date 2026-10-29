// CPU-01-STARTUP-SLOT-569 (lane m6): native model of startup constructor slot 569 (RVA 0x1cc80, 491 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 569. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_569.py. NOTHING here
// executes, loads or calls an original RVA: vtables, functor tags and the destructors 0x18a60 / 0x272d0 are
// IDENTITY TAGS, recorded and never run.
//
// Observed shape (stack canary present, 32-byte aligned frame):
//   1. `xchg [0x2dfc8a0], rax` with rax = 0: a sequentially consistent atomic store of 0 into a qword cell.
//      No atexit is registered for it. It is the first effect of the body.
//   2. The ranged INT console variable `mem_platformMemVerify` at 0x2dfc8b0, built by 0xc580 (core/
//      ranged_int_var_c580): min 0, max 2, flags 1, empty description, value_ref = var + 0xd0, caller functor tag
//      0x1594c70; afterwards `[var] = 0x1594bf8 + 0x10`, value 0 / default 0, 0x20d60 registration, two 0xf590
//      view releases and __cxa_atexit(0x18a60, var).
//   3. The debug-menu toggle at 0x2dfc988 (= var + 0xd8): label `Memory`, text `PS4 Platform MemVerify (0=off)`,
//      target = the variable, flags 0x30 (0x26ea0, tail-registered through 0x268b0), f590(text), f590(label),
//      __cxa_atexit(0x272d0, item).
// New fact proven by the checker (not by earlier lanes): the caller tag 0x1594c70 has the same four relocation
// slots (0xc720 / 0xc740 / 0xc760 / 0xc770) as the already-lifted int functor class 0x15944a0, so the shared
// 0xc580 model applies unchanged; the helper validates the REAL tag and hands the equivalent accepted tag over
// (the constructor never stores the caller tag). The int class answers the toggle's two virtual reads with
// {1, 2}: the gate `slot3 == 1 && slot6 == 3` is false, so the toggle keeps its default range {1.0f, 10.0f}
// (ranged_int_c580_menu_target).
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot569Rva = 0x1cc80;
inline constexpr std::size_t kSlot569Size = 491;
inline constexpr std::uint64_t kSlot569IntFnTag = 0x1594c70;
inline constexpr char kSlot569VarName[] = "mem_platformMemVerify";
inline constexpr std::size_t kSlot569VarNameLength = 21;
inline constexpr char kSlot569MenuLabel[] = "Memory";
inline constexpr char kSlot569MenuText[] = "PS4 Platform MemVerify (0=off)";
inline constexpr std::int32_t kSlot569Min = 0;
inline constexpr std::int32_t kSlot569Max = 2;

struct Slot569Objects {
    std::uint64_t cell;       // 0x2dfc8a0 (atomic store of 0)
    ConCmdRangedVar var;      // 0x2dfc8b0
    DebugMenuItem item;       // 0x2dfc988
};

struct Slot569Result {
    bool ok = false;
    bool cell_stored = false;
    lane_m6::IntVarResult var{};
    MenuSiteResult toggle{};
};

[[nodiscard]] Slot569Result init_slot569_1cc80(Slot569Objects& objects, ConCmdRegistry& concmd,
                                               DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
