#include "arena_high_water.hpp"

#include <cstring>

namespace sotc {
namespace {

std::uint64_t load_u64(void* arena, std::size_t off) noexcept {
    std::uint64_t v = 0;
    std::memcpy(&v, static_cast<char*>(arena) + off, sizeof(v));
    return v;
}

void store_u64(void* arena, std::size_t off, std::uint64_t v) noexcept {
    std::memcpy(static_cast<char*>(arena) + off, &v, sizeof(v));
}

void add_and_peak(void* arena, std::size_t used_off, std::size_t peak_off,
                  std::uint64_t delta) noexcept {
    const std::uint64_t used = load_u64(arena, used_off) + delta;  // u64 wrap = OBS
    store_u64(arena, used_off, used);
    // OBS jbe: skip peak store when used <= peak (unsigned).
    if (used > load_u64(arena, peak_off)) {
        store_u64(arena, peak_off, used);
    }
}

}  // namespace

void arena_add_used_high_water(void* arena, std::uint64_t delta) noexcept {
    if (!arena) return;
    // OBS order: +0x90/+0x98 first, then +0x80/+0x88.
    add_and_peak(arena, kArenaOffUsed90, kArenaOffPeak98, delta);
    add_and_peak(arena, kArenaOffUsed80, kArenaOffPeak88, delta);
}

}  // namespace sotc
