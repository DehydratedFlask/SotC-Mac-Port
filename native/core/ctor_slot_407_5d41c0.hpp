// CPU-01-STARTUP-SLOTS-M2 (lane M2): native model of startup constructor slot 407 (RVA 0x5d41c0, 374 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 407. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_407.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: the bool variable `cp10_creditsUseNewList` (object 0x2ff59c0, flags 1, functor tag 0x15cb258, value and
// default 1), then two constant statics WITHOUT atexit: a 12-byte zero vec3 (0x2ff649c) and a 4x4 float identity
// matrix (0x2ff64ac, 64 bytes; {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1} row/column order is immaterial). The 4 bytes at
// 0x2ff64a8 between them are never written. What consumes the vector and matrix is NOT recovered here.
#pragma once

#include <array>

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot407Rva = 0x5d41c0;
inline constexpr std::size_t kSlot407Size = 374;
inline constexpr char kSlot407VarName[] = "cp10_creditsUseNewList";
inline constexpr std::uint64_t kSlot407FnTag = 0x15cb258;
inline constexpr std::uint32_t kSlot407One = 0x3f800000u;   // 1.0f bit pattern, the four diagonal dwords

struct Slot407Objects {
    ConCmdBoolVar var;                      // 0x2ff59c0
    std::array<std::uint32_t, 3> vec3;      // 0x2ff649c, three zero dwords
    std::array<std::uint32_t, 16> matrix;   // 0x2ff64ac, identity
};

struct Slot407Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot407Result init_slot407_5d41c0(Slot407Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
