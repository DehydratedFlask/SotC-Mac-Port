#include <cstdio>
#include <cstdlib>

#include "core/vec_d3a6d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD3a6d0 g{1, 2, 3, 4};
    init_vec_d3a6d0(g);
    CHECK(g == kVecD3a6d0Bits);

    g = {9, 9, 9, 9};
    init_vec_d3a6d0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD3a6d0Bits[0], kVecD3a6d0Bits[1], kVecD3a6d0Bits[2], kVecD3a6d0Bits[3],
                    0x13016d0ull, 0x3352090ull, 0xd3a6d0ull, 113u);
    } else {
        std::puts("vec_d3a6d0: all checks passed");
    }
    return 0;
}
