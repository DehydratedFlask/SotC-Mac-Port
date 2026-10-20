#include <cstdio>
#include <cstdlib>

#include "core/vec_neg036.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecNeg036 g{};
    g.lanes = {1, 2, 3, 4};
    init_vec_neg036(g);
    CHECK(g.guard == 1);
    for (auto lane : g.lanes) CHECK(lane == kNeg036Bits);

    // Guarded: second call must not rewrite lanes.
    g.lanes = {9, 9, 9, 9};
    init_vec_neg036(g);
    CHECK(g.guard == 1);
    for (auto lane : g.lanes) CHECK(lane == 9);

    // Nonzero low byte in a wider guard word still skips (original compares byte [guard]).
    VecNeg036 g2{};
    g2.guard = 0x100;  // low byte 0 → still initializes
    init_vec_neg036(g2);
    CHECK(g2.guard == 1 && g2.lanes[0] == kNeg036Bits);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u]}\n", kNeg036Bits, kNeg036Bits, kNeg036Bits, kNeg036Bits);
    } else {
        std::puts("vec_neg036: all checks passed");
    }
    return 0;
}
