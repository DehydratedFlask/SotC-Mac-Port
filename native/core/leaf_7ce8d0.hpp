// Native recovery of original ctor RVA 0x7ce8d0 (ctor index 346; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (41 bytes, 8 instructions):
// two plain 64-bit stores of the constant 0x3f80000000000001 (dwords {1, 1.0f}) and two *atomic*
// 32-bit zero stores (`xor ecx,ecx; xchg [cell],ecx`, a locked exchange whose old value is
// discarded) into BSS cells 0x325e470..0x325e4a4. The cells are 0x10 apart:
//   +0x00 u64 (plain)   +0x10 u32 (atomic)   +0x20 u64 (plain)   +0x30 u32 (atomic)
// The gaps (+0x08..+0x10, +0x14..+0x20, +0x28..+0x30) are never written.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace sotc {

struct Leaf7ce8d0 {
    std::uint64_t word0;                          // +0x00  plain store
    std::array<std::uint8_t, 8> unwritten0;       // +0x08  never written
    std::atomic<std::uint32_t> flag0;             // +0x10  atomic exchange-store of 0
    std::array<std::uint8_t, 12> unwritten1;      // +0x14  never written
    std::uint64_t word1;                          // +0x20  plain store
    std::array<std::uint8_t, 8> unwritten2;       // +0x28  never written
    std::atomic<std::uint32_t> flag1;             // +0x30  atomic exchange-store of 0
};

// Original cells 0x325e470..0x325e4a4 are the first 0x34 bytes of Leaf7ce8d0.
inline constexpr std::size_t kLeaf7ce8d0RegionBytes = 0x34;
// Cell offsets the original writes with `xchg` (full-barrier atomic) rather than a plain `mov`.
inline constexpr std::array<std::size_t, 2> kLeaf7ce8d0AtomicOffsets = {0x10, 0x30};

// Behaviour of 0x7ce8d0, in the original order: word0 = 0x3f80000000000001; flag0.exchange(0);
// word1 = 0x3f80000000000001; flag1.exchange(0) (both exchanges sequentially consistent).
void init_leaf_7ce8d0(Leaf7ce8d0& g) noexcept;

}  // namespace sotc
