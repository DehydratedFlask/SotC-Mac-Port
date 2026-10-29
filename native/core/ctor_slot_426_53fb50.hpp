// CPU-01-STARTUP-SLOT-426 (lane m3): native model of startup constructor slot 426 (RVA 0x53fb50, 336 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 426. Every body fact below was read
// end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_426.py. NOTHING here executes,
// loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded
// (the atexit log) and never run.
//
// Shape, in order: (1) bool variable `showGameStateValues` (object 0x2eff370, tag 0x15c8e98, value 0, default 0, flags 1) -- the
// inline init_bool_var_static sequence ending in __cxa_atexit(0x20b0); (2) one zeroed 16-byte static at 0x2eff438
// (`vmovups xmm0(0) -> [cell]`) with __cxa_atexit(0x539a00, cell, &dso). Observation: 0x539a00 (83 B) reads a control
// block pointer at cell+8, drops its use count with `lock xadd` and, on the last reference, calls control-block
// virtual slots +0x10 and +0x18: the destructor of a std::shared_ptr-like {ptr, ctl} pair. Inference only: the cell is
// an empty shared pointer. The destructor is body-hash-pinned and RECORDED, never run (its indirect calls are
// unreachable for the empty cell and are not modelled).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot426Rva = 0x53fb50;
inline constexpr std::size_t kSlot426Size = 336;
inline constexpr char kSlot426Name[] = "showGameStateValues";
inline constexpr std::uint64_t kSlot426FnTag = 0x15c8e98;
inline constexpr std::uint64_t kSlot426ObjectRva = 0x2eff370;
inline constexpr std::uint64_t kSlot426CellRva = 0x2eff438;
inline constexpr std::uint64_t kSlot426PairDtorRva = 0x539a00;
inline constexpr std::size_t kSlot426PairDtorSize = 83;

struct Slot426Pair16 {
    std::uint64_t ptr;   // +0x00
    std::uint64_t ctl;   // +0x08 control block pointer read by the destructor
};
static_assert(sizeof(Slot426Pair16) == 0x10);

struct Slot426Objects {
    ConCmdBoolVar var;    // 0x2eff370
    Slot426Pair16 cell;     // 0x2eff438
};

struct Slot426Result {
    bool ok = false;
    BoolVarResult var{};
    bool cell_atexit = false;
};

[[nodiscard]] Slot426Result init_slot426_53fb50(Slot426Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
