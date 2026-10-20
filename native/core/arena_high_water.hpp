// SRV-01: arena used-byte counters + high-water peaks after list surgery
// (OBS 0x1a9bb..0x1a9fc). Distinct from arena_accounting (0x1a123 mode block).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

// OBS layout on arena (r15):
//   +0x80 running used-bytes A; +0x88 peak A (unsigned max)
//   +0x90 running used-bytes B; +0x98 peak B (unsigned max)
inline constexpr std::size_t kArenaHighWaterMinBytes = 0xa0;
inline constexpr std::size_t kArenaOffUsed80 = 0x80;
inline constexpr std::size_t kArenaOffPeak88 = 0x88;
inline constexpr std::size_t kArenaOffUsed90 = 0x90;
inline constexpr std::size_t kArenaOffPeak98 = 0x98;

// 0x1a9bb..0x1a9fc: used90 += delta; if used90 > peak98 then peak98 = used90;
//                   used80 += delta; if used80 > peak88 then peak88 = used80.
// Comparison is unsigned (jbe). delta is rbx at the OBS site.
void arena_add_used_high_water(void* arena, std::uint64_t delta) noexcept;

}  // namespace sotc
