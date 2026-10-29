// CPU-01-STARTUP-SLOTS-M4 (lane M4): native model of startup constructor slot 479 (RVA 0x31b660, 299 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 479 is at 0x2dd70e0 - 479 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_479.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable `uiWarnOnUnhandledEvents` (length 23, value 0, default 0, flags 1, empty description, functor
// tag 0x15a6eb0) at object 0x2ef03d8: 0x20af0, f590(description), 0x20d60, f590(name), __cxa_atexit(0x20b0, obj,
// &dso) -- the slot-348 shape, native as init_bool_var_site. OBSERVATION only: the image shows the construction;
// what reads the variable is NOT modelled here.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot479Rva = 0x31b660;
inline constexpr std::size_t kSlot479Size = 299;
inline constexpr char kSlot479Name[] = "uiWarnOnUnhandledEvents";
inline constexpr std::uint64_t kSlot479FnTag = 0x15a6eb0;
inline constexpr std::uint64_t kSlot479ObjectRva = 0x2ef03d8;
inline constexpr std::uint8_t kSlot479Value = 0;
inline constexpr std::uint8_t kSlot479Default = 0;

struct Slot479Objects {
    ConCmdBoolVar var;   // 0x2ef03d8
};

struct Slot479Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot479Result init_slot479_31b660(Slot479Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
