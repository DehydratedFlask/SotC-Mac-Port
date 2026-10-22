// Native recovery of original ctor RVA 0xe71990 (ctor index 23; ELF sha256 d0723afb...5861d3).
// Unguarded, call-free constructor body (337 bytes, 43 instructions): fills a table of 14 records
// of 16 bytes at BSS 0x3353f40..0x335401c with
//     +0x00  u64  address of a NUL-terminated name string in the original read-only data
//     +0x08  u32  the name hash of that string (immediate in the code)
//     +0x0c       four bytes never written
// The hashes are constants of the original code. The 13 non-zero ones equal
// sotc::name_hash(name) of the 13 original strings (docs/checkpoints/2026-10-21-agentB-namehash-e71990.md);
// the 14th record's hash is the literal 0 (sentinel). The 14 string addresses are original image
// addresses with no meaning in a native process and the strings are original game content, so
// they are NOT embedded here: the caller supplies the 14 name pointers (as 64-bit values) in record
// order. Which strings the game should use natively is a separate data-policy decision (not made
// here). Names are ours; the original has no symbols.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kLeafE71990Records = 14;

struct LeafE71990 {
    // Original cells 0x3353f40..0x335401c (220 bytes; 168 covered, the 4 padding bytes after each of
    // the first 13 hashes are never written).
    std::array<std::uint8_t, 220> cells;
};

struct LeafE71990Inputs {
    // Value stored at +0x00 of each record (in the original: the image address of the name string).
    std::array<std::uint64_t, kLeafE71990Records> name_address;
};

// Behaviour of 0xe71990: for record k (cells + 0x10*k): name_address[k] -> +0x00 (little endian),
// kLeafE71990Hash[k] -> +0x08, in the original store order.
void init_leaf_e71990(LeafE71990& g, const LeafE71990Inputs& in) noexcept;

// The 14 hash immediates of the original code, in record order (last one is the 0 sentinel).
inline constexpr std::array<std::uint32_t, kLeafE71990Records> kLeafE71990Hash = {
    0xf1e58049u, 0x257da323u, 0x47a2c413u, 0xed7e6ae8u, 0xb2d47dcdu, 0x4941f51cu, 0x4941f51du,
    0x4941f51eu, 0x4941f51fu, 0x10ef7257u, 0x7cae59c9u, 0x330bbb24u, 0x2b1488e7u, 0x00000000u,
};

}  // namespace sotc
