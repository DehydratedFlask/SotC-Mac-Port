#include <cstdio>
#include <cstdlib>

#include "core/vec_dc4180.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDc4180 g{1, 2, 3, 4};
    init_vec_dc4180(g);
    CHECK(g == kVecDc4180Bits);

    g = {9, 9, 9, 9};
    init_vec_dc4180(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDc4180Bits[0], kVecDc4180Bits[1], kVecDc4180Bits[2], kVecDc4180Bits[3],
                    0x130e220ull, 0x3352cc0ull, 0xdc4180ull, 68u);
    } else {
        std::puts("vec_dc4180: all checks passed");
    }
    return 0;
}
