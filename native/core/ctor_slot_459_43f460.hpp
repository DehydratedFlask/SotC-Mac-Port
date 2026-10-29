// CPU-01-STARTUP-SLOT-459 (lane m3): native model of startup constructor slot 459 (RVA 0x43f460, 299 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 459. Every body fact below was read
// end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_459.py. NOTHING here executes,
// loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded
// (the atexit log) and never run.
//
// Shape: ONE bool console variable `SM_printAnimPhase` (value 0, default 0, flags 1, functor tag 0x15b70d0, object 0x2ef7f30,
// empty description): the inline 0x20af0 / 0x20d60 / 0xf590 / atexit(0x20b0) sequence already native as
// init_bool_var_static. In this body the object is addressed rip-relative (no base register), which changes the
// encoding (299 bytes) but not a single store: base ctor, f590(description), 0x20d60, f590(name), atexit(0x20b0).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot459Rva = 0x43f460;
inline constexpr std::size_t kSlot459Size = 299;
inline constexpr char kSlot459Name[] = "SM_printAnimPhase";
inline constexpr std::uint64_t kSlot459FnTag = 0x15b70d0;
inline constexpr std::uint64_t kSlot459ObjectRva = 0x2ef7f30;

struct Slot459Objects {
    ConCmdBoolVar var;
};

struct Slot459Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot459Result init_slot459_43f460(Slot459Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
