// Native recovery of original ctor RVA 0xea52f0 (index 22; ELF sha256 d0723afb...5861d3).
// Pure data init: 184-byte BSS image @0x3354020 (OBS store simulation). No calls.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kInitEa52f0Bytes = 184;

using InitEa52f0 = std::array<std::uint8_t, kInitEa52f0Bytes>;

void init_ea52f0(InitEa52f0& g) noexcept;

}  // namespace sotc
