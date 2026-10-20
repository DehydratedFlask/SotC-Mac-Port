// SRV-01: exact-fit consume finalize OBS 0x1aada..0x1aaf3 (no 0x19660).
// Distinct from carve math (size selection) and region_split (remainder neu).
#pragma once

#include <cstdint>

#include "region_split.hpp"

namespace sotc {

// Arena dword observed at +0x78; copied into node flag_a (+0x68) on exact fit.
inline constexpr std::size_t kArenaFlagWordOffset = 0x78;

// 0x1aada..0x1aaf3 on the existing free node (r14):
//   flag_a (+0x68) = low byte of [arena+0x78]
//   size   (+0x58) = req_size (rbx)
//   flag_b (+0x69) = 1
//   ptr    (+0x40) = ctx (rsi; may be 0)
//   sentinel (+0x48) = 0xffffffff
// start/cap untouched here. Does not enter page-map @0x1ab08.
void exact_fit_finalize(RegionNode& node, std::uint64_t req_size, std::uint8_t arena_flag,
                        std::uint64_t ctx) noexcept;

}  // namespace sotc
