// CPU-01-STARTUP-SLOT-539 (lane m6): native model of startup constructor slot 539 (RVA 0x121940, 571 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 539. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_539.py (the destructors
// 0x112d80 and 0x8ef00 are hash-pinned there). NOTHING here executes, loads or calls an original RVA: vtables and
// destructors are IDENTITY TAGS, recorded and never run.
//
// Observed shape (stack canary present; the four objects are contiguous from 0x2edf140):
//   1. 0x2edf140: a zeroed qword cell, then __cxa_atexit(0x112d80, cell). 0x112d80 is `if (*cell) { 0x113690(*cell);
//      0x170b0(*cell); *cell = 0 }`: with the cell zero it does nothing; it is only recorded here.
//   2. 0x2edf148: a 0x30-byte NAME HOLDER: [+0] = 0x15976a0 + 0x10, [+8..+0x28) = a SharedStr view
//      {"r_showBounds", 0xc, 0, 0} (control == nullptr, borrowed), [+0x28] = 0; then f590(stack view) (exact no-op)
//      and __cxa_atexit(0x8ef00, holder). 0x8ef00 sets [holder] = 0x1597648 + 0x10 and tail-jumps 0xf590 on holder+8.
//   3. 0x2edf178: the bool console variable `r_showObjectCount` (name length 0x11, empty description at 0xfa6c3d),
//      functor tag 0x1598020, value 0 / default 0, built in the init_bool_var_static order, atexit 0x20b0.
//   4. 0x2edf240 (= variable + 0xc8): a second name holder {"r_threadRObjectNewAndFree", 0x19}, same shape,
//      __cxa_atexit(0x8ef00, holder).
// Inference (not observed): the holders are named debug-flag handles for the renderer; no consumer is lifted here.
// The contiguity of the four objects is observed; the native struct below keeps their order and 0x100-byte extent
// so the original offsets are reproducible, but nothing relies on adjacency.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot539Rva = 0x121940;
inline constexpr std::size_t kSlot539Size = 571;
inline constexpr std::uint64_t kSlot539CellDtorRva = 0x112d80;
inline constexpr std::uint64_t kSlot539HolderDtorRva = 0x8ef00;
inline constexpr std::uint64_t kSlot539HolderVptr = 0x15976b0;   // 0x15976a0 + 0x10 (identity tag)
inline constexpr std::uint64_t kSlot539BoolFnTag = 0x1598020;
inline constexpr char kSlot539Holder1Name[] = "r_showBounds";
inline constexpr std::size_t kSlot539Holder1NameLength = 0xc;
inline constexpr char kSlot539VarName[] = "r_showObjectCount";
inline constexpr std::size_t kSlot539VarNameLength = 0x11;
inline constexpr char kSlot539Holder2Name[] = "r_threadRObjectNewAndFree";
inline constexpr std::size_t kSlot539Holder2NameLength = 0x19;

struct NameHolder30 {
    std::uint64_t vptr;   // +0x00
    SharedStr name;       // +0x08 .. +0x28
    std::uint64_t tail;   // +0x28 = 0
};
static_assert(sizeof(NameHolder30) == 0x30 && offsetof(NameHolder30, name) == 8 && offsetof(NameHolder30, tail) == 0x28);

struct Slot539Objects {
    std::uint64_t cell;           // 0x2edf140
    NameHolder30 holder1;         // 0x2edf148
    ConCmdBoolVar var;            // 0x2edf178
    NameHolder30 holder2;         // 0x2edf240
};
static_assert(offsetof(Slot539Objects, holder1) == 0x8 && offsetof(Slot539Objects, var) == 0x38 &&
              offsetof(Slot539Objects, holder2) == 0x100);

struct Slot539Result {
    bool ok = false;
    bool cell_atexit = false;      // (0x112d80, cell) recorded
    bool holder1_ok = false;       // holder built, f590 inert, (0x8ef00, holder1) recorded
    BoolVarResult var{};
    bool holder2_ok = false;
};

[[nodiscard]] Slot539Result init_slot539_121940(Slot539Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
