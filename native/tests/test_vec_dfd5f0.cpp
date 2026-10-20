#include <cstdio>
#include <cstdlib>

#include "core/vec_dfd5f0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDfd5f0 g{1, 2, 3, 4};
    init_vec_dfd5f0(g);
    CHECK(g == kVecDfd5f0Bits);

    g = {9, 9, 9, 9};
    init_vec_dfd5f0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDfd5f0Bits[0], kVecDfd5f0Bits[1], kVecDfd5f0Bits[2], kVecDfd5f0Bits[3],
                    0x13124a0ull, 0x3353340ull, 0xdfd5f0ull, 54u);
    } else {
        std::puts("vec_dfd5f0: all checks passed");
    }
    return 0;
}
