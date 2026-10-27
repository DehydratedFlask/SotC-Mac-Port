// CPU-01-STARTUP-SLOTS-343-356 (lane L4): native model of startup constructor slot 345 (RVA 0x7cec60, 431 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dda750 + (306 - 345) * 8. Every body fact
// below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_345.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape (the slot 303 shape): bool variable `shadow_debugPlayerControllerState` (object 0x325e4b8, tag
// 0x15ce390, value 0, default 0) + a 0x26ea0 toggle (item 0x325e580, flags 0x30) labelled `Shadow Player` /
// `Debug Player Controller State`. Releases: text first, then label; then atexit(0x272d0).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot345Rva = 0x7cec60;
inline constexpr std::size_t kSlot345Size = 431;
inline constexpr char kSlot345Name[] = "shadow_debugPlayerControllerState";
inline constexpr char kSlot345Label[] = "Shadow Player";
inline constexpr char kSlot345Text[] = "Debug Player Controller State";
inline constexpr std::uint64_t kSlot345FnTag = 0x15ce390;

struct Slot345Objects {
    ConCmdBoolVar var;
    DebugMenuItem item;
};

struct Slot345Result {
    bool ok = false;
    BoolVarResult var{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot345Result init_slot345_7cec60(Slot345Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
