// Independent oracles: sinl/cosl on the exact float input rounded once to float (<= 1 ULP), the value range
// cos(3 deg) = 0.998629..., and sin^2 + cos^2 = 1. The range check is what would catch swapped sin/cos offsets.
#include <bit>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>

#include "core/libm_policy.hpp"
#include "core/trig_constants.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

static long ulp_distance(float a, float b) {
    auto key = [](float f) { auto u = std::bit_cast<std::int32_t>(f); return u < 0 ? std::int32_t(0x80000000u) - u : u; };
    return std::labs(static_cast<long>(key(a)) - static_cast<long>(key(b)));
}

int main(int argc, char**) {
    const float x = std::bit_cast<float>(sotc::kThreeDegreesBits);
    CHECK(std::fabs(x - 3.0f * 3.14159265358979f / 180.0f) < 1e-7f);     // really 3 degrees

    sotc::TrigConstants g{};
    sotc::init_trig_constants(g);
    const float want_cos = static_cast<float>(cosl(static_cast<long double>(x)));
    const float want_sin = static_cast<float>(sinl(static_cast<long double>(x)));
    sotc::TrigConstants preseeded{{0x12345678u, 0x9abcdef0u, 0x13579bdfu, 0x2468ace0u}, -123.0f};
    sotc::init_trig_constants(preseeded);
    CHECK((preseeded.xyz_mask == std::array<std::uint32_t, 4>{~0u, ~0u, ~0u, 0}));
    CHECK(ulp_distance(preseeded.cos_3_degrees, want_cos) <= 1);
    CHECK(ulp_distance(g.cos_3_degrees, want_cos) <= 1);
    CHECK(ulp_distance(sotc::libm::fsin(x, 0), want_sin) <= 1);
    CHECK(sotc::libm::fsin(x, 0) != sotc::libm::fsin(x, 1));
    const float s = sotc::libm::fsin(x, 0), c = g.cos_3_degrees;
    CHECK(std::fabs(s * s + c * c - 1.0f) < 1e-6f);
    CHECK(g.cos_3_degrees > 0.9986f && g.cos_3_degrees < 0.9987f);       // cos 3 deg = 0.998629...
    CHECK((g.xyz_mask == std::array<std::uint32_t, 4>{~0u, ~0u, ~0u, 0}));

    if (argc > 1) std::printf("{\"x_bits\":%u,\"offset\":1}\n", sotc::kThreeDegreesBits);
    else std::puts("trig_constants: all checks passed");
    return 0;
}
