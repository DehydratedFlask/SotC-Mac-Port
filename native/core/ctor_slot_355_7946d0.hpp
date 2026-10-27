// CPU-01-STARTUP-SLOTS-343-356 (lane L4): native model of startup constructor slot 355 (RVA 0x7946d0, 646 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dda750 + (306 - 355) * 8. Every body fact
// below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_355.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: two bool variables (`shadow_yamoriBossDead` object 0x3259270, `shadow_yamoriBossInfiniteStun` object
// 0x3259338; both tag 0x15ce050, value 0, default 0, shared empty description 0x1284c3b) + a 0x26ea0 toggle
// (item 0x3259400, flags 0x30) that targets the SECOND variable: `Shadow AI` / `Yamori Boss Infinite Stun`.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot355Rva = 0x7946d0;
inline constexpr std::size_t kSlot355Size = 646;
inline constexpr char kSlot355NameA[] = "shadow_yamoriBossDead";
inline constexpr char kSlot355NameB[] = "shadow_yamoriBossInfiniteStun";
inline constexpr char kSlot355Label[] = "Shadow AI";
inline constexpr char kSlot355Text[] = "Yamori Boss Infinite Stun";
inline constexpr std::uint64_t kSlot355FnTag = 0x15ce050;

struct Slot355Objects {
    ConCmdBoolVar var_a;
    ConCmdBoolVar var_b;
    DebugMenuItem item;
};

struct Slot355Result {
    bool ok = false;
    BoolVarResult var_a{};
    BoolVarResult var_b{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot355Result init_slot355_7946d0(Slot355Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
