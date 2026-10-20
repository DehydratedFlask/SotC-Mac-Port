#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/arena_high_water.hpp"

#define CHECK(c)                                                                          \
    do {                                                                                  \
        if (!(c)) {                                                                       \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);               \
            std::exit(1);                                                                 \
        }                                                                                 \
    } while (0)

static std::uint64_t load(const unsigned char* a, std::size_t off) {
    std::uint64_t v = 0;
    std::memcpy(&v, a + off, 8);
    return v;
}

static void store(unsigned char* a, std::size_t off, std::uint64_t v) {
    std::memcpy(a + off, &v, 8);
}

int main() {
    using namespace sotc;
    CHECK(kArenaHighWaterMinBytes >= 0xa0);

    alignas(8) unsigned char arena[0xa0]{};

    // Fresh: peaks rise with used.
    sotc::arena_add_used_high_water(arena, 0x1000);
    CHECK(load(arena, kArenaOffUsed90) == 0x1000);
    CHECK(load(arena, kArenaOffPeak98) == 0x1000);
    CHECK(load(arena, kArenaOffUsed80) == 0x1000);
    CHECK(load(arena, kArenaOffPeak88) == 0x1000);

    // Add again: used grows; peaks follow.
    sotc::arena_add_used_high_water(arena, 0x200);
    CHECK(load(arena, kArenaOffUsed90) == 0x1200);
    CHECK(load(arena, kArenaOffPeak98) == 0x1200);
    CHECK(load(arena, kArenaOffUsed80) == 0x1200);
    CHECK(load(arena, kArenaOffPeak88) == 0x1200);

    // Simulate used drop without peak drop (allocator can lower used elsewhere).
    store(arena, kArenaOffUsed90, 0x100);
    store(arena, kArenaOffUsed80, 0x100);
    sotc::arena_add_used_high_water(arena, 0x50);
    CHECK(load(arena, kArenaOffUsed90) == 0x150);
    CHECK(load(arena, kArenaOffPeak98) == 0x1200);  // peak unchanged (jbe path)
    CHECK(load(arena, kArenaOffUsed80) == 0x150);
    CHECK(load(arena, kArenaOffPeak88) == 0x1200);

    // Equal to peak: jbe skips (unsigned <=).
    store(arena, kArenaOffUsed90, 0x1200);
    store(arena, kArenaOffUsed80, 0x1200);
    sotc::arena_add_used_high_water(arena, 0);
    CHECK(load(arena, kArenaOffPeak98) == 0x1200);
    CHECK(load(arena, kArenaOffPeak88) == 0x1200);

    // Null arena is a no-op.
    sotc::arena_add_used_high_water(nullptr, 1);

    std::puts("arena_high_water: all checks passed");
    return 0;
}
