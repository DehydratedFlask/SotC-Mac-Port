#include <cstdio>
#include <cstdlib>

#include "core/vec_d20590.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD20590 g{1, 2, 3, 4};
    init_vec_d20590(g);
    CHECK(g == kVecD20590Bits);

    g = {9, 9, 9, 9};
    init_vec_d20590(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD20590Bits[0], kVecD20590Bits[1], kVecD20590Bits[2], kVecD20590Bits[3],
                    0x12ff030ull, 0x3351e90ull, 0xd20590ull, 123u);
    } else {
        std::puts("vec_d20590: all checks passed");
    }
    return 0;
}
