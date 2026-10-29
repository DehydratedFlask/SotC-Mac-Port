// CPU-01-STARTUP-SLOTS-M4 (lane M4): native model of startup constructor slot 476 (RVA 0x329650, 323 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 476 is at 0x2dd70e0 - 476 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_476.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable `r_showFogVolumes` (length 16, value 0, default 0, flags 1, empty description, functor
// tag 0x15a7c50) at object 0x2ef0888: 0x20af0, f590(description), 0x20d60, f590(name), __cxa_atexit(0x20b0, obj,
// &dso) -- the slot-348 shape, native as init_bool_var_site. OBSERVATION only: the image shows the construction;
// what reads the variable is NOT modelled here.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot476Rva = 0x329650;
inline constexpr std::size_t kSlot476Size = 323;
inline constexpr char kSlot476Name[] = "r_showFogVolumes";
inline constexpr std::uint64_t kSlot476FnTag = 0x15a7c50;
inline constexpr std::uint64_t kSlot476ObjectRva = 0x2ef0888;
inline constexpr std::uint8_t kSlot476Value = 0;
inline constexpr std::uint8_t kSlot476Default = 0;

struct Slot476Objects {
    ConCmdBoolVar var;   // 0x2ef0888
};

struct Slot476Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot476Result init_slot476_329650(Slot476Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
