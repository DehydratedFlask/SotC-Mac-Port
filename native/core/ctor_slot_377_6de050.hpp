// CPU-01-STARTUP-SLOT-377 (lane M1): native model of startup constructor slot 377 (RVA 0x6de050, 299 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 377 (checker-pinned). Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by
// tools/lift/check_ctor_slot_377.py. NOTHING here executes, loads or calls an original RVA: vtable pointers,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable (value 1, default 1, flags 1) -- the inline 0x20af0 / 0x20d60 / 0xf590 /
// atexit(0x20b0) sequence already native as init_bool_var_site. Object 0x3240cd0, name `pauseGameIfControllerDisconnected`
// (length 0x21), empty description, functor tag 0x15cc998. Sequence: 0x20af0, f590(description), 0x20d60,
// f590(name), __cxa_atexit(0x20b0, obj, 0x1604218). Observation only; the variable's consumers are not recovered.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot377Rva = 0x6de050;
inline constexpr std::size_t kSlot377Size = 299;
inline constexpr char kSlot377Name[] = "pauseGameIfControllerDisconnected";
inline constexpr std::uint64_t kSlot377FnTag = 0x15cc998;
inline constexpr std::uint64_t kSlot377ObjectRva = 0x3240cd0;

struct Slot377Objects {
    ConCmdBoolVar var;
};

struct Slot377Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot377Result init_slot377_6de050(Slot377Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
