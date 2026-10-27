// CPU-01-STARTUP-SLOTS-343-356 (lane L4): native model of startup constructor slot 348 (RVA 0x7c9150, 299 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dda750 + (306 - 348) * 8. Every body fact
// below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_348.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable (value 1, default 1, flags 1) -- the inline 0x20af0 / 0x20d60 / 0xf590 /
// atexit(0x20b0) sequence already native as init_bool_var_static. Object 0x325dd78, name `cp3_shadow_enableDebugMove`
// (length 26), empty description, functor tag 0x15ce2f8. Sequence: 0x20af0, f590(description), 0x20d60,
// f590(name), __cxa_atexit(0x20b0, obj, 0x1604218).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot348Rva = 0x7c9150;
inline constexpr std::size_t kSlot348Size = 299;
inline constexpr char kSlot348Name[] = "cp3_shadow_enableDebugMove";
inline constexpr std::uint64_t kSlot348FnTag = 0x15ce2f8;
inline constexpr std::uint64_t kSlot348ObjectRva = 0x325dd78;

struct Slot348Objects {
    ConCmdBoolVar var;
};

struct Slot348Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot348Result init_slot348_7c9150(Slot348Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
