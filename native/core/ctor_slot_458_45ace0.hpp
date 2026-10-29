// CPU-01-STARTUP-SLOT-458 (lane m3): native model of startup constructor slot 458 (RVA 0x45ace0, 323 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 458. Every body fact below was read
// end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_458.py. NOTHING here executes,
// loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded
// (the atexit log) and never run.
//
// Shape: ONE bool console variable `showFootDrivenMotion` (value 0, default 0, flags 1, functor tag 0x15bab98, object 0x2ef7ff8,
// empty description): the inline 0x20af0 / 0x20d60 / 0xf590 / atexit(0x20b0) sequence already native as
// init_bool_var_static. In this body the object is addressed rip-relative (no base register), which changes the
// encoding (323 bytes) but not a single store: base ctor, f590(description), 0x20d60, f590(name), atexit(0x20b0).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot458Rva = 0x45ace0;
inline constexpr std::size_t kSlot458Size = 323;
inline constexpr char kSlot458Name[] = "showFootDrivenMotion";
inline constexpr std::uint64_t kSlot458FnTag = 0x15bab98;
inline constexpr std::uint64_t kSlot458ObjectRva = 0x2ef7ff8;

struct Slot458Objects {
    ConCmdBoolVar var;
};

struct Slot458Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot458Result init_slot458_45ace0(Slot458Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
