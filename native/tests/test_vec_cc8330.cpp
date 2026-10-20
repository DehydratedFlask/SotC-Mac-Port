#include <cstdio>
#include <cstdlib>

#include "core/vec_cc8330.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCc8330 g{1, 2, 3, 4};
    init_vec_cc8330(g);
    CHECK(g == kVecCc8330Bits);

    g = {9, 9, 9, 9};
    init_vec_cc8330(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCc8330Bits[0], kVecCc8330Bits[1], kVecCc8330Bits[2], kVecCc8330Bits[3],
                    0x12f5fd0ull, 0x3351c10ull, 0xcc8330ull, 163u);
    } else {
        std::puts("vec_cc8330: all checks passed");
    }
    return 0;
}
