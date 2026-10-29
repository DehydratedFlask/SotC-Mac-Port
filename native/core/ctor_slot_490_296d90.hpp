// CPU-01-STARTUP-SLOTS-M4 (lane M4): native model of startup constructor slot 490 (RVA 0x296d90, 323 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 490 is at 0x2dd70e0 - 490 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_490.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable `r_showLights` (length 12, value 0, default 0, flags 1, empty description, functor
// tag 0x159e430) at object 0x2eef5e8: 0x20af0, f590(description), 0x20d60, f590(name), __cxa_atexit(0x20b0, obj,
// &dso) -- the slot-348 shape, native as init_bool_var_site. OBSERVATION only: the image shows the construction;
// what reads the variable is NOT modelled here.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot490Rva = 0x296d90;
inline constexpr std::size_t kSlot490Size = 323;
inline constexpr char kSlot490Name[] = "r_showLights";
inline constexpr std::uint64_t kSlot490FnTag = 0x159e430;
inline constexpr std::uint64_t kSlot490ObjectRva = 0x2eef5e8;
inline constexpr std::uint8_t kSlot490Value = 0;
inline constexpr std::uint8_t kSlot490Default = 0;

struct Slot490Objects {
    ConCmdBoolVar var;   // 0x2eef5e8
};

struct Slot490Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot490Result init_slot490_296d90(Slot490Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
