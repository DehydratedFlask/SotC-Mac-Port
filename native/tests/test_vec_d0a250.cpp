#include <cstdio>
#include <cstdlib>

#include "core/vec_d0a250.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD0a250 g{1, 2, 3, 4};
    init_vec_d0a250(g);
    CHECK(g == kVecD0a250Bits);

    g = {9, 9, 9, 9};
    init_vec_d0a250(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD0a250Bits[0], kVecD0a250Bits[1], kVecD0a250Bits[2], kVecD0a250Bits[3],
                    0x12fd3b0ull, 0x3351e10ull, 0xd0a250ull, 131u);
    } else {
        std::puts("vec_d0a250: all checks passed");
    }
    return 0;
}
