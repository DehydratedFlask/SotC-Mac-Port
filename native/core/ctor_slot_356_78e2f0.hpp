// CPU-01-STARTUP-SLOTS-343-356 (lane L4): native model of startup constructor slot 356 (RVA 0x78e2f0, 299 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dda750 + (306 - 356) * 8. Every body fact
// below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_356.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable (value 1, default 1, flags 1) -- the inline 0x20af0 / 0x20d60 / 0xf590 /
// atexit(0x20b0) sequence already native as init_bool_var_static. Object 0x3256390, name `shadow_weapons_newWeaponCyclingLogic`
// (length 36), empty description, functor tag 0x15cdfb0. Sequence: 0x20af0, f590(description), 0x20d60,
// f590(name), __cxa_atexit(0x20b0, obj, 0x1604218).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot356Rva = 0x78e2f0;
inline constexpr std::size_t kSlot356Size = 299;
inline constexpr char kSlot356Name[] = "shadow_weapons_newWeaponCyclingLogic";
inline constexpr std::uint64_t kSlot356FnTag = 0x15cdfb0;
inline constexpr std::uint64_t kSlot356ObjectRva = 0x3256390;

struct Slot356Objects {
    ConCmdBoolVar var;
};

struct Slot356Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot356Result init_slot356_78e2f0(Slot356Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
