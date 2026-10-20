#include <cstdio>
#include <cstdlib>

#include "core/vec_dd41e0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDd41e0 g{};
    g.lanes = {1, 2, 3, 4};
    init_vec_dd41e0(g);
    CHECK(g.guard == 1);
    for (auto lane : g.lanes) CHECK(lane == kNeg036BitsDd41);

    g.lanes = {9, 9, 9, 9};
    init_vec_dd41e0(g);
    CHECK(g.guard == 1);
    for (auto lane : g.lanes) CHECK(lane == 9);

    VecDd41e0 g2{};
    g2.guard = 0x100;  // low byte 0 → still initializes
    init_vec_dd41e0(g2);
    CHECK(g2.guard == 1 && g2.lanes[0] == kNeg036BitsDd41);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"guard\":%llu,\"value\":%llu,\"source\":%llu}\n",
                    kNeg036BitsDd41, kNeg036BitsDd41, kNeg036BitsDd41, kNeg036BitsDd41,
                    0x3352ce0ull, 0x3352cd0ull, 0x130eac0ull);
    } else {
        std::puts("vec_dd41e0: all checks passed");
    }
    return 0;
}
