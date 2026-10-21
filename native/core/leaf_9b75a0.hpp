// Native recovery of original ctor RVA 0x9b75a0 (ctor index 319; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (50 bytes, 10 instructions): a
// 256-iteration loop that zeroes 28 bytes of each 0x28-byte record of a BSS table (two overlapping
// 16-byte stores per record, +0x00 and +0x0c relative to the record's cleared window) followed by a
// 4-byte zero at +0x2800 of the table. Names are ours; the original has no symbols.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kLeaf9b75a0Records = 256;
inline constexpr std::size_t kLeaf9b75a0Stride = 0x28;
inline constexpr std::size_t kLeaf9b75a0Cells = 0x27fc;

struct Leaf9b75a0 {
    // Original cells 0x32ef798..0x32f1f94 (= table base 0x32ef790 + 8 .. + 0x2804); the table's
    // first 8 bytes, the 12 bytes between cleared record windows and the 4 bytes at table +0x27fc
    // are never written. Offset 0 here is table offset 8.
    std::array<std::uint8_t, kLeaf9b75a0Cells> cells;
};

// Behaviour of 0x9b75a0: zero the 28-byte window of every record and the trailing dword.
void init_leaf_9b75a0(Leaf9b75a0& g) noexcept;

}  // namespace sotc
