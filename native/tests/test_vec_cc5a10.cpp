#include <cstdio>
#include <cstdlib>

#include "core/vec_cc5a10.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCc5a10 g{1, 2, 3, 4};
    init_vec_cc5a10(g);
    CHECK(g == kVecCc5a10Bits);

    g = {9, 9, 9, 9};
    init_vec_cc5a10(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCc5a10Bits[0], kVecCc5a10Bits[1], kVecCc5a10Bits[2], kVecCc5a10Bits[3],
                    0x12f5c80ull, 0x3351c00ull, 0xcc5a10ull, 164u);
    } else {
        std::puts("vec_cc5a10: all checks passed");
    }
    return 0;
}
