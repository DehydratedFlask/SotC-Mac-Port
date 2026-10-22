// 0xdf94c0 stores cos(3 deg) (from the same input bits as 0xe10490) into one float cell.
// Independent oracle: cosl on the exact float input rounded once to float (<= 1 ULP), the value range
// 0.998629..., and equality with the already-verified 0xe10490 lift's result. --dump feeds
// tools/lift/check_fdf94c0.py (binary-vs-native tie).
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/leaf_df94c0.hpp"
#include "core/trig_constants.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

static long ulp_distance(float a, float b) {
    auto key = [](float f) { auto u = std::bit_cast<std::int32_t>(f); return u < 0 ? std::int32_t(0x80000000u) - u : u; };
    return std::labs(static_cast<long>(key(a)) - static_cast<long>(key(b)));
}

int main(int argc, char**) {
    const float x = std::bit_cast<float>(sotc::kThreeDegreesBits);
    const float want = static_cast<float>(cosl(static_cast<long double>(x)));
    static_assert(sizeof(sotc::LeafDf94c0) == 4, "single 4-byte cell");

    sotc::LeafDf94c0 g{};
    sotc::init_leaf_df94c0(g);
    CHECK(ulp_distance(g.cos_3_degrees, want) <= 1);
    CHECK(g.cos_3_degrees > 0.9986f && g.cos_3_degrees < 0.9987f);

    // The cell is overwritten whatever it held (unguarded), and idempotent.
    for (float seed : {-123.0f, 0.0f, 7.0f}) {
        sotc::LeafDf94c0 p{seed};
        sotc::init_leaf_df94c0(p);
        CHECK(std::memcmp(&p.cos_3_degrees, &g.cos_3_degrees, 4) == 0);
        sotc::init_leaf_df94c0(p);
        CHECK(std::memcmp(&p.cos_3_degrees, &g.cos_3_degrees, 4) == 0);
    }
    // Same value as the verified 0xe10490 lift (same input bits, same _FSin arguments).
    sotc::TrigConstants t{};
    sotc::init_trig_constants(t);
    CHECK(std::memcmp(&t.cos_3_degrees, &g.cos_3_degrees, 4) == 0);

    if (argc > 1) std::printf("{\"x_bits\":%u,\"offset\":1,\"result_bits\":%u}\n", sotc::kThreeDegreesBits,
                              std::bit_cast<std::uint32_t>(g.cos_3_degrees));
    else std::puts("leaf_df94c0: all checks passed");
    return 0;
}
