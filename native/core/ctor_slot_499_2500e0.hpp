// CPU-01-STARTUP-SLOTS-M5 (lane M5): native model of startup constructor slot 499 (RVA 0x2500e0, 323 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 499 is at 0x2dd70e0 - 499 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_499.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable `r_showDirectionalBoxLights` (length 26, value 0, default 0, flags 1, empty description, functor
// tag 0x159c318) at object 0x2eebc58: 0x20af0, f590(description), 0x20d60, f590(name), __cxa_atexit(0x20b0, obj,
// &dso) -- the slot-348 shape, native as init_bool_var_site. OBSERVATION only: the image shows the construction;
// what reads the variable is NOT modelled here.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot499Rva = 0x2500e0;
inline constexpr std::size_t kSlot499Size = 323;
inline constexpr char kSlot499Name[] = "r_showDirectionalBoxLights";
inline constexpr std::uint64_t kSlot499FnTag = 0x159c318;
inline constexpr std::uint64_t kSlot499ObjectRva = 0x2eebc58;

struct Slot499Objects {
    ConCmdBoolVar var;   // 0x2eebc58
};

struct Slot499Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot499Result init_slot499_2500e0(Slot499Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
