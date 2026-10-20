#include <cstdio>
#include <cstdlib>

#include "core/vec_d7fe90.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD7fe90 g{1, 2, 3, 4};
    init_vec_d7fe90(g);
    CHECK(g == kVecD7fe90Bits);

    g = {9, 9, 9, 9};
    init_vec_d7fe90(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD7fe90Bits[0], kVecD7fe90Bits[1], kVecD7fe90Bits[2], kVecD7fe90Bits[3],
                    0x1307dc0ull, 0x3352a50ull, 0xd7fe90ull, 86u);
    } else {
        std::puts("vec_d7fe90: all checks passed");
    }
    return 0;
}
