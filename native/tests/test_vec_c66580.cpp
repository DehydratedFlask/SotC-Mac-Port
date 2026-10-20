#include <cstdio>
#include <cstdlib>

#include "core/vec_c66580.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC66580 g{1, 2, 3, 4};
    init_vec_c66580(g);
    CHECK(g == kVecC66580Bits);

    g = {9, 9, 9, 9};
    init_vec_c66580(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC66580Bits[0], kVecC66580Bits[1], kVecC66580Bits[2], kVecC66580Bits[3],
                    0x12f0160ull, 0x3351750ull, 0xc66580ull, 194u);
    } else {
        std::puts("vec_c66580: all checks passed");
    }
    return 0;
}
