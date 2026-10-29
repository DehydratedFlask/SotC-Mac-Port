// CPU-01-STARTUP-SLOT-558 (lane m6): native model of startup constructor slot 558 (RVA 0x428a0, 230 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 558. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_558.py. NOTHING here
// executes, loads or calls an original RVA: the three destructors (0x418c0, 0x41900, 0x41910) are IDENTITY TAGS,
// hash-pinned by the checker, recorded in the atexit log and never run.
//
// Observed shape (no stack canary, no console objects, no strings):
//   * five zeroed qword cells at 0x2ea86a0, 0x2ea86a8, 0x2ea86b0, 0x2ea86b8, 0x2ea86c0; after each store the body
//     calls __cxa_atexit(0x418c0, &cell, &dso) (0x418c0 reaches the helpers 0x170c0 / 0x170b0);
//   * an inline small vector `F` at 0x2ea86f8 (0x160 bytes): begin = end = F + 0x20 (one 16-byte store of the
//     broadcast pointer), the end-of-storage pointer at F + 0x10 = F + 0x160, dword gap at F + 0x18 never written,
//     inline storage F + 0x20 .. F + 0x160 (0x140 bytes) never written; then __cxa_atexit(0x41900, F, &dso) where
//     0x41900 is a bare `ret` (hash-pinned);
//   * a zeroed qword `G` at 0x2ea8858 (= F + 0x160, which is the byte after F's inline storage) followed by a TAIL
//     `jmp __cxa_atexit(0x41910, G, &dso)`; 0x41910 deletes the owned object through a virtual slot [vtbl + 8] and
//     stores 0 back (G is an owning pointer cell).
// The cells/F/G are separate native members (the five cells end at 0x2ea86c8 and F starts at 0x2ea86f8, so they are
// not adjacent; no adjacency claim). Native pointers replace the original absolute addresses.
// Atexit registration order: cell0..cell4, F, G.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot558Rva = 0x428a0;
inline constexpr std::size_t kSlot558Size = 230;
inline constexpr std::uint64_t kSlot558CellDtorRva = 0x418c0;
inline constexpr std::uint64_t kSlot558VectorDtorRva = 0x41900;
inline constexpr std::uint64_t kSlot558OwnerDtorRva = 0x41910;
inline constexpr std::size_t kSlot558CellCount = 5;
inline constexpr std::size_t kSlot558VectorStorageBytes = 0x140;

struct Slot558InlineVector {
    std::uint8_t* begin;          // +0x00 = &storage
    std::uint8_t* end;            // +0x08 = &storage
    std::uint8_t* storage_end;    // +0x10 = &storage + 0x140
    std::uint8_t gap18[8];        // +0x18 never written
    std::uint8_t storage[kSlot558VectorStorageBytes];   // +0x20 never written
};
static_assert(sizeof(Slot558InlineVector) == 0x160 && offsetof(Slot558InlineVector, storage) == 0x20);

struct Slot558Objects {
    std::array<std::uint64_t, kSlot558CellCount> cells;   // 0x2ea86a0 .. 0x2ea86c0
    Slot558InlineVector vector;                           // 0x2ea86f8
    std::uint64_t owner;                                  // 0x2ea8858
};

struct Slot558Result {
    bool ok = false;
    std::size_t atexit_recorded = 0;   // seven registrations when everything succeeded
};

[[nodiscard]] Slot558Result init_slot558_428a0(Slot558Objects& objects, ConCmdRegistry& registry) noexcept;

}  // namespace sotc
