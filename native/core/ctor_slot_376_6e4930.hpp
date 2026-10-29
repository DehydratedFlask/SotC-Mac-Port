// CPU-01-STARTUP-SLOT-376 (lane M1): native model of startup constructor slot 376 (RVA 0x6e4930, 299 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 376 (checker-pinned). Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by
// tools/lift/check_ctor_slot_376.py. NOTHING here executes, loads or calls an original RVA: vtable pointers,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bool console variable (value 0, default 0, flags 1) -- the inline 0x20af0 / 0x20d60 / 0xf590 /
// atexit(0x20b0) sequence already native as init_bool_var_site. Object 0x32415d0, name `skippableEndSequence`
// (length 0x14), empty description, functor tag 0x15cc9b8. Sequence: 0x20af0, f590(description), 0x20d60,
// f590(name), __cxa_atexit(0x20b0, obj, 0x1604218). Observation only; the variable's consumers are not recovered.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot376Rva = 0x6e4930;
inline constexpr std::size_t kSlot376Size = 299;
inline constexpr char kSlot376Name[] = "skippableEndSequence";
inline constexpr std::uint64_t kSlot376FnTag = 0x15cc9b8;
inline constexpr std::uint64_t kSlot376ObjectRva = 0x32415d0;

struct Slot376Objects {
    ConCmdBoolVar var;
};

struct Slot376Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot376Result init_slot376_6e4930(Slot376Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
