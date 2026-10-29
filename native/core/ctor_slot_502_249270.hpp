// CPU-01-STARTUP-SLOTS-M5 (lane M5): native model of startup constructor slot 502 (RVA 0x249270, 323 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 502 is at 0x2dd70e0 - 502 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_502.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable `r_showProjectedDecals` (length 21, value 0, default 0, flags 1, empty description, functor
// tag 0x159bc00) at object 0x2eeae10: 0x20af0, f590(description), 0x20d60, f590(name), __cxa_atexit(0x20b0, obj,
// &dso) -- the slot-348 shape, native as init_bool_var_site. OBSERVATION only: the image shows the construction;
// what reads the variable is NOT modelled here.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot502Rva = 0x249270;
inline constexpr std::size_t kSlot502Size = 323;
inline constexpr char kSlot502Name[] = "r_showProjectedDecals";
inline constexpr std::uint64_t kSlot502FnTag = 0x159bc00;
inline constexpr std::uint64_t kSlot502ObjectRva = 0x2eeae10;

struct Slot502Objects {
    ConCmdBoolVar var;   // 0x2eeae10
};

struct Slot502Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot502Result init_slot502_249270(Slot502Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
