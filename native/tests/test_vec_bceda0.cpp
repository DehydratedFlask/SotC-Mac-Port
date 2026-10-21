#include <cstdio>
#include <cstdlib>

#include "core/vec_bceda0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBCEDA0 g{1, 2, 3, 4};
    init_vec_bceda0(g);
    CHECK(g == kVecBCEDA0Bits);

    g = {9, 9, 9, 9};
    init_vec_bceda0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBCEDA0Bits[0], kVecBCEDA0Bits[1], kVecBCEDA0Bits[2], kVecBCEDA0Bits[3],
                    0x12e0850ull, 0x3351150ull, 0xbceda0ull, 249u);
    } else {
        std::puts("vec_bceda0: all checks passed");
    }
    return 0;
}
