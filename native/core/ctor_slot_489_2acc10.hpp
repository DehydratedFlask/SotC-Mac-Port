// CPU-01-STARTUP-SLOTS-M4 (lane M4): native model of startup constructor slot 489 (RVA 0x2acc10, 342 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 489 is at 0x2dd70e0 - 489 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_489.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE unbounded int console variable `disableInput` (object 0x2eef6b0, length 12, value 0, default 0,
// flags 1, empty description) built by the existing 0x3a980 worker with the argument-functor tag 0x159fae0, then
// f590(description), the bare-ret destroy through the tag's slot 3, 0x20d60, f590(name), __cxa_atexit(0x18a60).
// No debug-menu item follows. The tag 0x159fae0 is accepted by the shared worker whitelist after its four slot
// relocations were pinned to the same bodies (0xc720 / 0xc740 / 0xc760 / 0xc770) as 0x15ce8c0 (see
// lane_m4_shapes.hpp). OBSERVATION only: what reads the variable is NOT modelled here.
#pragma once

#include "lane_m4_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot489Rva = 0x2acc10;
inline constexpr std::size_t kSlot489Size = 342;
inline constexpr char kSlot489Name[] = "disableInput";
inline constexpr std::uint64_t kSlot489ArgFnTag = 0x159fae0;
inline constexpr std::uint64_t kSlot489ObjectRva = 0x2eef6b0;
inline constexpr std::uint32_t kSlot489Value = 0;
inline constexpr std::uint32_t kSlot489Default = 0;

struct Slot489Objects {
    ConCmdRangedVar var;   // 0x2eef6b0
};

struct Slot489Result {
    bool ok = false;
    lane_m4::IntVarSiteResult var{};
};

[[nodiscard]] Slot489Result init_slot489_2acc10(Slot489Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
