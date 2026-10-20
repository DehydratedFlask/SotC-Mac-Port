#include <cstdio>
#include <cstdlib>

#include "core/vec_c4aa80.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC4AA80 g{1, 2, 3, 4};
    init_vec_c4aa80(g);
    CHECK(g == kVecC4AA80Bits);

    g = {9, 9, 9, 9};
    init_vec_c4aa80(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC4AA80Bits[0], kVecC4AA80Bits[1], kVecC4AA80Bits[2], kVecC4AA80Bits[3],
                    0x12edcc0ull, 0x3351650ull, 0xc4aa80ull, 210u);
    } else {
        std::puts("vec_c4aa80: all checks passed");
    }
    return 0;
}
