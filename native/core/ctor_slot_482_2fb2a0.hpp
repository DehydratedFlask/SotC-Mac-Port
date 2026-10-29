// CPU-01-STARTUP-SLOTS-M4 (lane M4): native model of startup constructor slot 482 (RVA 0x2fb2a0, 323 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 482 is at 0x2dd70e0 - 482 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_482.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable `network_WarnAboutServerObjectsOnClient` (length 38, value 1, default 1, flags 1, empty description, functor
// tag 0x15a52f8) at object 0x2eefd30: 0x20af0, f590(description), 0x20d60, f590(name), __cxa_atexit(0x20b0, obj,
// &dso) -- the slot-348 shape, native as init_bool_var_site. OBSERVATION only: the image shows the construction;
// what reads the variable is NOT modelled here.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot482Rva = 0x2fb2a0;
inline constexpr std::size_t kSlot482Size = 323;
inline constexpr char kSlot482Name[] = "network_WarnAboutServerObjectsOnClient";
inline constexpr std::uint64_t kSlot482FnTag = 0x15a52f8;
inline constexpr std::uint64_t kSlot482ObjectRva = 0x2eefd30;
inline constexpr std::uint8_t kSlot482Value = 1;
inline constexpr std::uint8_t kSlot482Default = 1;

struct Slot482Objects {
    ConCmdBoolVar var;   // 0x2eefd30
};

struct Slot482Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot482Result init_slot482_2fb2a0(Slot482Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
