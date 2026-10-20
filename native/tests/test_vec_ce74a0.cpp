#include <cstdio>
#include <cstdlib>

#include "core/vec_ce74a0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCe74a0 g{1, 2, 3, 4};
    init_vec_ce74a0(g);
    CHECK(g == kVecCe74a0Bits);

    g = {9, 9, 9, 9};
    init_vec_ce74a0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCe74a0Bits[0], kVecCe74a0Bits[1], kVecCe74a0Bits[2], kVecCe74a0Bits[3],
                    0x12f8d90ull, 0x3351d00ull, 0xce74a0ull, 148u);
    } else {
        std::puts("vec_ce74a0: all checks passed");
    }
    return 0;
}
