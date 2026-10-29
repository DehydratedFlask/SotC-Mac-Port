// CPU-01-STARTUP-SLOTS-M4 (lane M4): native model of startup constructor slot 478 (RVA 0x31eb00, 285 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 478 is at 0x2dd70e0 - 478 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_478.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable `uiTextShowExtents` (length 17, value 0, default 0, flags 1, functor tag 0x15a7260) at
// object 0x2ef04a0 WITH a 69-byte description. This is the late-release variant (lane_m4::init_bool_var_late_release):
// 0x20af0, the field stores (no 0x15940b0 mid vptr), 0x20d60, then f590(description) and f590(name) BOTH after the
// registration, then __cxa_atexit(0x20b0, obj, &dso). The description text is the image's own string; what reads
// the variable (an extents overlay switch, by its name and text) is NOT modelled here.
#pragma once

#include "lane_m4_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot478Rva = 0x31eb00;
inline constexpr std::size_t kSlot478Size = 285;
inline constexpr char kSlot478Name[] = "uiTextShowExtents";
inline constexpr char kSlot478Description[] = "Draws a semi-opque quad behind each text element showing its extents.";
inline constexpr std::uint64_t kSlot478FnTag = 0x15a7260;
inline constexpr std::uint64_t kSlot478ObjectRva = 0x2ef04a0;
inline constexpr std::uint8_t kSlot478Value = 0;
inline constexpr std::uint8_t kSlot478Default = 0;

struct Slot478Objects {
    ConCmdBoolVar var;   // 0x2ef04a0
};

struct Slot478Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot478Result init_slot478_31eb00(Slot478Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
