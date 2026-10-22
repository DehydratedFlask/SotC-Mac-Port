// Independent oracles: IEEE-754 identities from <limits>/<numbers>, not copies of the recovered bits.
// Also prints the produced bit patterns as JSON for tools/lift/check_f6cad0.py (binary-vs-native tie).
#include <array>
#include <bit>
#include <cstdint>
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

    // Four distinct guard profiles exercise each value/guard pair on both branches.
    {
        constexpr std::array<std::uint8_t, 8> signatures{1, 2, 3, 4, 5, 6, 7, 8};
        const std::array<float, 6> float_sentinels{
            101.125f, 102.125f, 103.125f, 104.125f, 105.125f, 106.125f};
        const std::array<std::uint32_t, 4> lane0_sentinel{
            0x11000001u, 0x11000002u, 0x11000003u, 0x11000004u};
        const std::array<std::uint32_t, 4> xyz_sentinel{
            0x22000001u, 0x22000002u, 0x22000003u, 0x22000004u};
        const std::array<std::uint32_t, 4> copy_sentinel{
            0x33000001u, 0x33000002u, 0x33000003u, 0x33000004u};
        const std::array<std::uint32_t, 4> lane0_oracle{~0u, 0, 0, 0};
        const std::array<std::uint32_t, 4> xyz_oracle{~0u, ~0u, ~0u, 0};

        const auto check_value = [](const auto& actual, const auto& sentinel,
                                    const auto& oracle, std::uint64_t actual_guard,
                                    std::uint64_t seeded_guard, bool preserve) {
            CHECK(actual == (preserve ? sentinel : oracle));
            CHECK(actual_guard == (preserve ? seeded_guard : 1));
        };

        for (unsigned pass_bit = 0; pass_bit < 4; ++pass_bit) {
            MathConstants state{};
            state.neg_one = float_sentinels[0];
            state.half = float_sentinels[1];
            state.two = float_sentinels[2];
            state.pi = float_sentinels[3];
            state.epsilon = float_sentinels[4];
            state.max_float = float_sentinels[5];
            state.lane0_mask = lane0_sentinel;
            state.xyz_mask = xyz_sentinel;
            state.xyz_mask_copy = copy_sentinel;

            const std::array<std::uint64_t*, 8> guards{
                &state.neg_one_guard, &state.half_guard, &state.two_guard, &state.pi_guard,
                &state.epsilon_guard, &state.max_float_guard, &state.lane0_mask_guard,
                &state.xyz_mask_guard};
            std::array<std::uint64_t, 8> seeded_guards{};
            const auto pass_mask = static_cast<std::uint8_t>(1u << pass_bit);
            for (std::size_t i = 0; i < signatures.size(); ++i) {
                const bool preserve = (signatures[i] & pass_mask) != 0;
                seeded_guards[i] = preserve ? 0x1201u + (i << 12) : 0x7f00u;
                *guards[i] = seeded_guards[i];
            }

            sotc::init_math_constants(state);
            check_value(state.neg_one, float_sentinels[0], -1.0f, state.neg_one_guard,
                        seeded_guards[0], (signatures[0] & pass_mask) != 0);
            check_value(state.half, float_sentinels[1], 0.5f, state.half_guard, seeded_guards[1],
                        (signatures[1] & pass_mask) != 0);
            check_value(state.two, float_sentinels[2], 2.0f, state.two_guard, seeded_guards[2],
                        (signatures[2] & pass_mask) != 0);
            check_value(state.pi, float_sentinels[3], std::numbers::pi_v<float>, state.pi_guard,
                        seeded_guards[3], (signatures[3] & pass_mask) != 0);
            check_value(state.epsilon, float_sentinels[4],
                        std::numeric_limits<float>::epsilon(), state.epsilon_guard,
                        seeded_guards[4], (signatures[4] & pass_mask) != 0);
            check_value(state.max_float, float_sentinels[5], std::numeric_limits<float>::max(),
                        state.max_float_guard, seeded_guards[5],
                        (signatures[5] & pass_mask) != 0);
            check_value(state.lane0_mask, lane0_sentinel, lane0_oracle, state.lane0_mask_guard,
                        seeded_guards[6], (signatures[6] & pass_mask) != 0);
            check_value(state.xyz_mask, xyz_sentinel, xyz_oracle, state.xyz_mask_guard,
                        seeded_guards[7], (signatures[7] & pass_mask) != 0);
            CHECK(state.xyz_mask_copy == xyz_oracle);
        }
    }

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
