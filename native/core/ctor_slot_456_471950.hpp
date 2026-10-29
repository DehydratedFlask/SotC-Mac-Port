// CPU-01-STARTUP-SLOT-456 (lane m3): native model of startup constructor slot 456 (RVA 0x471950, 323 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 456. Every body fact below was read
// end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_456.py. NOTHING here executes,
// loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded
// (the atexit log) and never run.
//
// Shape: ONE bool console variable `wwise_postSounds` (value 1, default 1, flags 1, functor tag 0x15bc748, object 0x2ef8188,
// empty description): the inline 0x20af0 / 0x20d60 / 0xf590 / atexit(0x20b0) sequence already native as
// init_bool_var_static. In this body the object is addressed rip-relative (no base register), which changes the
// encoding (323 bytes) but not a single store: base ctor, f590(description), 0x20d60, f590(name), atexit(0x20b0).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot456Rva = 0x471950;
inline constexpr std::size_t kSlot456Size = 323;
inline constexpr char kSlot456Name[] = "wwise_postSounds";
inline constexpr std::uint64_t kSlot456FnTag = 0x15bc748;
inline constexpr std::uint64_t kSlot456ObjectRva = 0x2ef8188;

struct Slot456Objects {
    ConCmdBoolVar var;
};

struct Slot456Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot456Result init_slot456_471950(Slot456Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
