#include <cstdio>
#include <cstdlib>

#include "core/vec_b44580.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB44580 g{1, 2, 3, 4};
    init_vec_b44580(g);
    CHECK(g == kVecB44580Bits);

    g = {9, 9, 9, 9};
    init_vec_b44580(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB44580Bits[0], kVecB44580Bits[1], kVecB44580Bits[2], kVecB44580Bits[3],
                    0x12d3880ull, 0x3350ca0ull, 0xb44580ull, 294u);
    } else {
        std::puts("vec_b44580: all checks passed");
    }
    return 0;
}
