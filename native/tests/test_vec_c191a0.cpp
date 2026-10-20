#include <cstdio>
#include <cstdlib>

#include "core/vec_c191a0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC191A0 g{1, 2, 3, 4};
    init_vec_c191a0(g);
    CHECK(g == kVecC191A0Bits);

    g = {9, 9, 9, 9};
    init_vec_c191a0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC191A0Bits[0], kVecC191A0Bits[1], kVecC191A0Bits[2], kVecC191A0Bits[3],
                    0x12e97a0ull, 0x3351540ull, 0xc191a0ull, 223u);
    } else {
        std::puts("vec_c191a0: all checks passed");
    }
    return 0;
}
