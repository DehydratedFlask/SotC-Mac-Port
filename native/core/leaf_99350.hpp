// Native recovery of original ctor RVA 0x99350 (ctor index 548; ELF sha256
// d0723afb...5861d3). Call-free constructor body (288 bytes, 38 instructions): 25 stores into the
// BSS cells 0x2ed3680..0x2ed37a4 that mix recovered constants (seven read-only tables (six 32-byte, one 16-byte) copied with
// ymm/xmm moves, small integers and flags) with two kinds of run-time input:
//   - a 24-byte object read from the BSS cell 0x2dd9fa8 (copied as 16 + 8 bytes), and
//   - the address 0x2dd9f84 of another BSS cell, stored as a 64-bit pointer.
// Neither input is a constant, so both are explicit parameters here: the caller supplies the object
// bytes and the 64-bit value to store (the original image address has no meaning in a native
// process). Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct Leaf99350 {
    // Original cells 0x2ed3680..0x2ed37a4 (292 bytes; the 13 bytes no store covers are never written).
    std::array<std::uint8_t, 292> cells;
};

struct Leaf99350Inputs {
    // Bytes of the original cell 0x2dd9fa8..0x2dd9fc0 at the time the constructor runs.
    std::array<std::uint8_t, 24> object;
    // The 64-bit value the original stores at 0x2ed3778 (there: the address 0x2dd9f84).
    std::uint64_t anchor;
};

// Behaviour of 0x99350: object[0..16) -> cells+0x100, object[16..24) -> cells+0x110,
// anchor -> cells+0xf8 (little endian), every other store a recovered constant.
void init_leaf_99350(Leaf99350& g, const Leaf99350Inputs& in) noexcept;

}  // namespace sotc
