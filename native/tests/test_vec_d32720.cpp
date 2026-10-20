#include <cstdio>
#include <cstdlib>

#include "core/vec_d32720.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD32720 g{1, 2, 3, 4};
    init_vec_d32720(g);
    CHECK(g == kVecD32720Bits);

    g = {9, 9, 9, 9};
    init_vec_d32720(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD32720Bits[0], kVecD32720Bits[1], kVecD32720Bits[2], kVecD32720Bits[3],
                    0x1300ba0ull, 0x3351ed0ull, 0xd32720ull, 119u);
    } else {
        std::puts("vec_d32720: all checks passed");
    }
    return 0;
}
