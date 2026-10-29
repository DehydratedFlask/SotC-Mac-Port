// CPU-01-STARTUP-SLOT-387 (lane M1): native model of startup constructor slot 387 (RVA 0x6acf00, 299 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 387 (checker-pinned). Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by
// tools/lift/check_ctor_slot_387.py. NOTHING here executes, loads or calls an original RVA: vtable pointers,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable (value 1, default 1, flags 1) -- the inline 0x20af0 / 0x20d60 / 0xf590 /
// atexit(0x20b0) sequence already native as init_bool_var_site. Object 0x3017890, name `CP3FileResource_copy_mem`
// (length 0x18), empty description, functor tag 0x15cc748. Sequence: 0x20af0, f590(description), 0x20d60,
// f590(name), __cxa_atexit(0x20b0, obj, 0x1604218). Observation only; the variable's consumers are not recovered.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot387Rva = 0x6acf00;
inline constexpr std::size_t kSlot387Size = 299;
inline constexpr char kSlot387Name[] = "CP3FileResource_copy_mem";
inline constexpr std::uint64_t kSlot387FnTag = 0x15cc748;
inline constexpr std::uint64_t kSlot387ObjectRva = 0x3017890;

struct Slot387Objects {
    ConCmdBoolVar var;
};

struct Slot387Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot387Result init_slot387_6acf00(Slot387Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
