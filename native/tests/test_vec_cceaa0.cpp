#include <cstdio>
#include <cstdlib>

#include "core/vec_cceaa0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCceaa0 g{1, 2, 3, 4};
    init_vec_cceaa0(g);
    CHECK(g == kVecCceaa0Bits);

    g = {9, 9, 9, 9};
    init_vec_cceaa0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCceaa0Bits[0], kVecCceaa0Bits[1], kVecCceaa0Bits[2], kVecCceaa0Bits[3],
                    0x12f67d0ull, 0x3351c40ull, 0xcceaa0ull, 160u);
    } else {
        std::puts("vec_cceaa0: all checks passed");
    }
    return 0;
}
