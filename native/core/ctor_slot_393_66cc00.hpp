// CPU-01-STARTUP-SLOTS-M2 (lane M2): native model of startup constructor slot 393 (RVA 0x66cc00, 299 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 393. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_393.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool variable `cp10_debugDrawFruit` (object 0x3012648, flags 1, functor tag 0x15cc5c8, value and
// default 0). No menu item, no other statics. The bytes after the body (0x66cd30..) are other functions.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot393Rva = 0x66cc00;
inline constexpr std::size_t kSlot393Size = 299;
inline constexpr char kSlot393VarName[] = "cp10_debugDrawFruit";
inline constexpr std::uint64_t kSlot393FnTag = 0x15cc5c8;

struct Slot393Objects {
    ConCmdBoolVar var;       // 0x3012648
};

struct Slot393Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot393Result init_slot393_66cc00(Slot393Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
