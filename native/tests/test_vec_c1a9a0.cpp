#include <cstdio>
#include <cstdlib>

#include "core/vec_c1a9a0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC1A9A0 g{1, 2, 3, 4};
    init_vec_c1a9a0(g);
    CHECK(g == kVecC1A9A0Bits);

    g = {9, 9, 9, 9};
    init_vec_c1a9a0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC1A9A0Bits[0], kVecC1A9A0Bits[1], kVecC1A9A0Bits[2], kVecC1A9A0Bits[3],
                    0x12e9a70ull, 0x3351550ull, 0xc1a9a0ull, 222u);
    } else {
        std::puts("vec_c1a9a0: all checks passed");
    }
    return 0;
}
