// CPU-01-STARTUP-SLOT-559 (lane m6): native model of startup constructor slot 559 (RVA 0x3f2d0, 456 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 559. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_559.py. NOTHING here
// executes, loads or calls an original RVA: the destructors 0x3d920 and 0x20b0 are IDENTITY TAGS (hash-pinned by
// the checker, recorded in the atexit log, never run).
//
// Observed shape (stack canary present; one `__cxa_atexit` before and one after the variable):
//   1. Object A at 0x2ea5170 = THREE inline small vectors of 0xc8-byte elements, 0x1158 bytes each (A + 0x0000,
//      A + 0x1158, A + 0x22b0; total 0x3408). Per vector V: begin = end = V + 0x28, end-of-storage V + 0x1158
//      (at V + 0x10), inline-begin pointer V + 0x28 stored at V + 0x20, V + 0x18 never written, storage
//      V + 0x28 .. V + 0x1158 (0x1130 bytes) never written. Built with `vmovq` + `vpshufd` + one 16-byte store.
//   2. __cxa_atexit(0x3d920, 0, &dso): NOTE the object argument is the null pointer (`xor esi, esi` at the head
//      of the body is never reassigned); 0x3d920 reads the global A directly, walks each vector in stride 0xc8
//      calling the element's virtual destructor, and frees a vector's heap block when begin != the inline
//      pointer (the free helper 0x170c0). The checker hash-pins 0x3d920.
//   3. The bool console variable `loggingIncludeTimestamp` (name length 0x17, empty description) at
//      0x2ea8578 (= A + 0x3408, the byte after A), functor tag 0x1595770, value 0 / default 0, flags 1, atexit
//      0x20b0. It is the same sequence as init_bool_var_static; the byte-pinned tag 0x1595770's four relocation
//      slots are the bool functor bodies (checker).
// Inference (not observed): the three vectors back a logging sink table; no consumer is lifted here.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot559Rva = 0x3f2d0;
inline constexpr std::size_t kSlot559Size = 456;
inline constexpr std::uint64_t kSlot559ObjectDtorRva = 0x3d920;
inline constexpr std::uint64_t kSlot559BoolFnTag = 0x1595770;
inline constexpr char kSlot559VarName[] = "loggingIncludeTimestamp";
inline constexpr std::size_t kSlot559VarNameLength = 0x17;
inline constexpr std::size_t kSlot559VectorBytes = 0x1158;
inline constexpr std::size_t kSlot559VectorStorageBytes = 0x1130;

struct Slot559Vector {
    std::uint8_t* begin;          // +0x00 = &storage
    std::uint8_t* end;            // +0x08 = &storage
    std::uint8_t* storage_end;    // +0x10 = end of this vector (V + 0x1158)
    std::uint8_t gap18[8];        // +0x18 never written
    std::uint8_t* inline_begin;   // +0x20 = &storage
    std::uint8_t storage[kSlot559VectorStorageBytes];   // +0x28 never written
};
static_assert(sizeof(Slot559Vector) == kSlot559VectorBytes && offsetof(Slot559Vector, storage) == 0x28);

struct Slot559Objects {
    std::array<Slot559Vector, 3> vectors;   // 0x2ea5170, +0x1158, +0x22b0
    ConCmdBoolVar var;                      // 0x2ea8578
};

struct Slot559Result {
    bool ok = false;
    bool object_atexit = false;   // (0x3d920, null) recorded
    BoolVarResult var{};
};

[[nodiscard]] Slot559Result init_slot559_3f2d0(Slot559Objects& objects, ConCmdRegistry& registry) noexcept;

}  // namespace sotc
