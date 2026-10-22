// Native recovery of original ctor RVA 0xde0160 (ctor index 59; ELF sha256 d0723afb...5861d3).
// Unguarded 60-byte body: memset(0x3352ee0, 1, 0x400) via PLT thunk 0xf8f4f0 (NID = memset),
// then vxorps/vmovups zeroes the adjacent 16 bytes at 0x33532e0 (= buffer + 0x400), then
// tail-jumps to __cxa_atexit via PLT thunk 0xf8f390 with (dtor=0xddf980, arg=0, dso=0x1604218).
// The registered destructor is a one-byte `ret` stub — EXPLICIT OMISSION of the atexit call
// (same policy as hashed_string_14c3a0 / registry_object_14c9a0 no-op dtors). Names are ours.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kLeafDe0160FillBytes = 0x400;
inline constexpr std::size_t kLeafDe0160TrailerBytes = 16;

struct LeafDe0160 {
    std::array<std::byte, kLeafDe0160FillBytes> fill{};       // original 0x3352ee0
    std::array<std::byte, kLeafDe0160TrailerBytes> trailer{}; // original 0x33532e0
};

// Rewrites both cells on every call (unguarded in the original). Does not register atexit.
void init_leaf_de0160(LeafDe0160& g) noexcept;

}  // namespace sotc
