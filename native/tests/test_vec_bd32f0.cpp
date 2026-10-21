#include <cstdio>
#include <cstdlib>

#include "core/vec_bd32f0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBD32F0 g{1, 2, 3, 4};
    init_vec_bd32f0(g);
    CHECK(g == kVecBD32F0Bits);

    g = {9, 9, 9, 9};
    init_vec_bd32f0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBD32F0Bits[0], kVecBD32F0Bits[1], kVecBD32F0Bits[2], kVecBD32F0Bits[3],
                    0x12e14c0ull, 0x3351180ull, 0xbd32f0ull, 246u);
    } else {
        std::puts("vec_bd32f0: all checks passed");
    }
    return 0;
}
