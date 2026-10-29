// CPU-01-STARTUP-SLOTS-M4 (lane M4): native model of startup constructor slot 475 (RVA 0x33cc60, 285 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 475 is at 0x2dd70e0 - 475 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_475.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable `uiTrackToTargetShowExtents` (length 26, value 0, default 0, flags 1, functor tag 0x15a93e8) at
// object 0x2ef0950 WITH a 71-byte description. This is the late-release variant (lane_m4::init_bool_var_late_release):
// 0x20af0, the field stores (no 0x15940b0 mid vptr), 0x20d60, then f590(description) and f590(name) BOTH after the
// registration, then __cxa_atexit(0x20b0, obj, &dso). The description text is the image's own string; what reads
// the variable (an extents overlay switch, by its name and text) is NOT modelled here.
#pragma once

#include "lane_m4_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot475Rva = 0x33cc60;
inline constexpr std::size_t kSlot475Size = 285;
inline constexpr char kSlot475Name[] = "uiTrackToTargetShowExtents";
inline constexpr char kSlot475Description[] = "Draws rectangles showing the extents of all track to target components.";
inline constexpr std::uint64_t kSlot475FnTag = 0x15a93e8;
inline constexpr std::uint64_t kSlot475ObjectRva = 0x2ef0950;
inline constexpr std::uint8_t kSlot475Value = 0;
inline constexpr std::uint8_t kSlot475Default = 0;

struct Slot475Objects {
    ConCmdBoolVar var;   // 0x2ef0950
};

struct Slot475Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot475Result init_slot475_33cc60(Slot475Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
