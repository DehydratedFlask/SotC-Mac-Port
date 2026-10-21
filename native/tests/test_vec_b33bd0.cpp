#include <cstdio>
#include <cstdlib>

#include "core/vec_b33bd0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB33BD0 g{1, 2, 3, 4};
    init_vec_b33bd0(g);
    CHECK(g == kVecB33BD0Bits);

    g = {9, 9, 9, 9};
    init_vec_b33bd0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB33BD0Bits[0], kVecB33BD0Bits[1], kVecB33BD0Bits[2], kVecB33BD0Bits[3],
                    0x12d2f50ull, 0x3350c70ull, 0xb33bd0ull, 295u);
    } else {
        std::puts("vec_b33bd0: all checks passed");
    }
    return 0;
}
