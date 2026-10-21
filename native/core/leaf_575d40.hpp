// Native recovery of original ctor RVA 0x575d40 (ctor index 417; ELF sha256
// d0723afb...5861d3). Call-free constructor body (29 bytes, 7 instructions): a 20-byte copy
// from the run-time BSS cell 0x2eea480 into the BSS cell 0x2f21520.
// Names are ours; the original has no symbols. The meaning of the two cells is not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

// Both cells are 20 bytes: a 16-byte block followed by a 4-byte tail (the original moves them
// with one 16-byte and one 4-byte load/store pair).
using Leaf575d40Cell = std::array<std::uint8_t, 20>;

// Behaviour of 0x575d40. The source is run-time state in the original (BSS, written by code that
// runs before this constructor), so it is an explicit parameter here, never an embedded constant.
// Like the original, both loads happen before either store (the 4-byte tail is stored first, then
// the 16-byte head), so passing the same object as source and destination is well defined.
void init_leaf_575d40(const Leaf575d40Cell& source, Leaf575d40Cell& dest) noexcept;

}  // namespace sotc
