// Independent oracles: IEEE-754 identities from <limits>/<numbers>, not copies of the recovered
// bits; the unguarded tail is checked against the exact bytes the original stores. --dump prints
// the produced cells plus their original addresses for tools/lift/check_fdd91b0.py (binary-vs-native tie).
#include <array>
#include <bit>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <numbers>

#include "core/guarded_dd91b0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
struct Cell { std::uint64_t guard; std::uint64_t value; std::uint32_t width; std::uint32_t bits[4]; };
// Original addresses and the bit patterns the original stores (metadata only).
constexpr Cell kCells[9] = {
    {0x3351ef8ull, 0x3351ef4ull, 4, {0xbf800000u, 0x00000000u, 0x00000000u, 0x00000000u}},
    {0x3351f08ull, 0x3351f00ull, 4, {0x3f000000u, 0x00000000u, 0x00000000u, 0x00000000u}},
    {0x3351f18ull, 0x3351f10ull, 4, {0x40000000u, 0x00000000u, 0x00000000u, 0x00000000u}},
    {0x3351f28ull, 0x3351f20ull, 4, {0x40490fdbu, 0x00000000u, 0x00000000u, 0x00000000u}},
    {0x3351f38ull, 0x3351f30ull, 4, {0x34000000u, 0x00000000u, 0x00000000u, 0x00000000u}},
    {0x3351f48ull, 0x3351f40ull, 4, {0x7f7fffffu, 0x00000000u, 0x00000000u, 0x00000000u}},
    {0x3351f60ull, 0x3351f50ull, 16, {0xffffffffu, 0x00000000u, 0x00000000u, 0x00000000u}},
    {0x3351f80ull, 0x3351f70ull, 16, {0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u}},
    {0x3351f90ull, 0x3351f88ull, 4, {0x00000001u, 0x00000000u, 0x00000000u, 0x00000000u}},
};

constexpr std::array<std::uint8_t, 16> kTail = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
};
}  // namespace
int main(int argc, char**) {
    using namespace sotc;
    GuardedDd91b0 g{};
    init_guarded_dd91b0(g);
    // Independent float identities.
    CHECK(g.neg_one == -1.0f);
    CHECK(g.half == 0.5f);
    CHECK(g.two == 2.0f);
    CHECK(g.pi == std::numbers::pi_v<float>);
    CHECK(g.epsilon == std::numeric_limits<float>::epsilon());
    CHECK(g.max_float == std::numeric_limits<float>::max());
    CHECK((g.lane0_mask == std::array<std::uint32_t, 4>{~0u, 0, 0, 0}));
    CHECK((g.xyz_mask == std::array<std::uint32_t, 4>{~0u, ~0u, ~0u, 0}));
    CHECK(g.one == 1);
    CHECK(g.neg_one_guard == 1 && g.lane0_mask_guard == 1 && g.one_guard == 1);
    CHECK(g.tail == kTail);

    // Guard semantics (observed): a nonzero low guard byte preserves the existing value; a guard
    // whose low byte is zero still initialises even with high bits set.
    GuardedDd91b0 h{};
    h.half = 9.0f; h.half_guard = 0x01;
    h.pi = 3.0f; h.pi_guard = 0x7f00;
    h.tail.fill(0);
    init_guarded_dd91b0(h);
    CHECK(h.half == 9.0f && h.half_guard == 0x01);
    CHECK(h.pi == std::numbers::pi_v<float> && h.pi_guard == 1);
    CHECK(h.tail == kTail);   // unguarded tail is always rewritten

    GuardedDd91b0 again = g;
    init_guarded_dd91b0(again);
    CHECK(again.pi == g.pi && again.xyz_mask == g.xyz_mask && again.tail == g.tail);

    if (argc > 1) {
        std::printf("{\"rva\":%u,\"body_length\":%u,\"cells\":[", 0xdd91b0u, 330u);
        for (int i = 0; i != 9; ++i) {
            std::printf("%s{\"guard\":%llu,\"value\":%llu,\"width\":%u,\"bits\":[",
                        i ? "," : "", static_cast<unsigned long long>(kCells[i].guard),
                        static_cast<unsigned long long>(kCells[i].value), kCells[i].width);
            for (std::uint32_t lane = 0; lane != kCells[i].width / 4; ++lane) {
                std::printf("%s%u", lane ? "," : "", kCells[i].bits[lane]);
            }
            std::printf("]}");
        }
        std::printf("],\"tail_base\":%llu,\"tail\":\"", static_cast<unsigned long long>(0x3352e90ull));
        for (auto byte : g.tail) std::printf("%02x", byte);
        std::printf("\"}\n");
    } else {
        std::puts("guarded_dd91b0: all checks passed");
    }
    return 0;
}
