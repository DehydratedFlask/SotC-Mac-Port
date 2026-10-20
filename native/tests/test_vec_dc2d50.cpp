#include <cstdio>
#include <cstdlib>

#include "core/vec_dc2d50.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDc2d50 g{1, 2, 3, 4};
    init_vec_dc2d50(g);
    CHECK(g == kVecDc2d50Bits);

    g = {9, 9, 9, 9};
    init_vec_dc2d50(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDc2d50Bits[0], kVecDc2d50Bits[1], kVecDc2d50Bits[2], kVecDc2d50Bits[3],
                    0x130e010ull, 0x3352cb0ull, 0xdc2d50ull, 69u);
    } else {
        std::puts("vec_dc2d50: all checks passed");
    }
    return 0;
}
