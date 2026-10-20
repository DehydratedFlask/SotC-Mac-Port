#include <cstdio>
#include <cstdlib>

#include "core/vec_ce38d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCe38d0 g{1, 2, 3, 4};
    init_vec_ce38d0(g);
    CHECK(g == kVecCe38d0Bits);

    g = {9, 9, 9, 9};
    init_vec_ce38d0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCe38d0Bits[0], kVecCe38d0Bits[1], kVecCe38d0Bits[2], kVecCe38d0Bits[3],
                    0x12f7fd0ull, 0x3351ca0ull, 0xce38d0ull, 154u);
    } else {
        std::puts("vec_ce38d0: all checks passed");
    }
    return 0;
}
