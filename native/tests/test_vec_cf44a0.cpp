#include <cstdio>
#include <cstdlib>

#include "core/vec_cf44a0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCf44a0 g{1, 2, 3, 4};
    init_vec_cf44a0(g);
    CHECK(g == kVecCf44a0Bits);

    g = {9, 9, 9, 9};
    init_vec_cf44a0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCf44a0Bits[0], kVecCf44a0Bits[1], kVecCf44a0Bits[2], kVecCf44a0Bits[3],
                    0x12fa4e0ull, 0x3351d80ull, 0xcf44a0ull, 140u);
    } else {
        std::puts("vec_cf44a0: all checks passed");
    }
    return 0;
}
