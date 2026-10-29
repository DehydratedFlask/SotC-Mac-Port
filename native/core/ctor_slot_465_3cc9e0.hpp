// CPU-01-STARTUP-SLOTS-M4 (lane M4): native model of startup constructor slot 465 (RVA 0x3cc9e0, 323 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 465 is at 0x2dd70e0 - 465 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_465.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable `showLoadPackages` (length 16, value 0, default 0, flags 1, empty description, functor
// tag 0x15b0fc8) at object 0x2ef6fe8: 0x20af0, f590(description), 0x20d60, f590(name), __cxa_atexit(0x20b0, obj,
// &dso) -- the slot-348 shape, native as init_bool_var_site. OBSERVATION only: the image shows the construction;
// what reads the variable is NOT modelled here.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot465Rva = 0x3cc9e0;
inline constexpr std::size_t kSlot465Size = 323;
inline constexpr char kSlot465Name[] = "showLoadPackages";
inline constexpr std::uint64_t kSlot465FnTag = 0x15b0fc8;
inline constexpr std::uint64_t kSlot465ObjectRva = 0x2ef6fe8;
inline constexpr std::uint8_t kSlot465Value = 0;
inline constexpr std::uint8_t kSlot465Default = 0;

struct Slot465Objects {
    ConCmdBoolVar var;   // 0x2ef6fe8
};

struct Slot465Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot465Result init_slot465_3cc9e0(Slot465Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
