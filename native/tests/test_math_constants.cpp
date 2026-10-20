// Independent oracles: IEEE-754 identities from <limits>/<numbers>, not copies of the recovered bits.
// Also prints the produced bit patterns as JSON for tools/lift/check_f6cad0.py (binary-vs-native tie).
#include <bit>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <numbers>

#include "core/math_constants.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using sotc::MathConstants;
    MathConstants g{};
    sotc::init_math_constants(g);
    CHECK(g.neg_one == -1.0f);
    CHECK(g.half == 0.5f);
    CHECK(g.two == 2.0f);
    CHECK(g.pi == std::numbers::pi_v<float>);
    CHECK(g.epsilon == std::numeric_limits<float>::epsilon());
    CHECK(g.max_float == std::numeric_limits<float>::max());
    CHECK((g.lane0_mask == std::array<uint32_t, 4>{~0u, 0, 0, 0}));
    CHECK((g.xyz_mask == std::array<uint32_t, 4>{~0u, ~0u, ~0u, 0}));
    CHECK(g.xyz_mask_copy == g.xyz_mask);
    CHECK(g.neg_one_guard == 1 && g.xyz_mask_guard == 1);

    // Guard semantics (observed): a nonzero low guard byte preserves an existing value...
    MathConstants h{};
    h.half = 9.0f; h.half_guard = 0x01;
    h.pi = 3.0f; h.pi_guard = 0x7f00;           // low byte zero -> init runs despite high bits
    h.xyz_mask_copy = {1, 2, 3, 4};             // unguarded copy is always rewritten
    sotc::init_math_constants(h);
    CHECK(h.half == 9.0f && h.half_guard == 0x01);
    CHECK(h.pi == std::numbers::pi_v<float> && h.pi_guard == 1);
    CHECK(h.xyz_mask_copy == g.xyz_mask);

    // Idempotent.
    MathConstants again = g;
    sotc::init_math_constants(again);
    CHECK(again.pi == g.pi && again.lane0_mask == g.lane0_mask);

    if (argc > 1) {   // machine-readable bit dump
        std::printf("{\"floats\":[%u,%u,%u,%u,%u,%u],\"lane0_mask\":[%u,%u,%u,%u],\"xyz_mask\":[%u,%u,%u,%u]}\n",
                    std::bit_cast<uint32_t>(g.neg_one), std::bit_cast<uint32_t>(g.half),
                    std::bit_cast<uint32_t>(g.two), std::bit_cast<uint32_t>(g.pi),
                    std::bit_cast<uint32_t>(g.epsilon), std::bit_cast<uint32_t>(g.max_float),
                    g.lane0_mask[0], g.lane0_mask[1], g.lane0_mask[2], g.lane0_mask[3],
                    g.xyz_mask[0], g.xyz_mask[1], g.xyz_mask[2], g.xyz_mask[3]);
    } else {
        std::puts("math_constants: all checks passed");
    }
    return 0;
}
