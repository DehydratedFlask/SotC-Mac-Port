#include <cstdio>
#include <cstdlib>

#include "core/vec_bab950.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBAB950 g{1, 2, 3, 4};
    init_vec_bab950(g);
    CHECK(g == kVecBAB950Bits);

    g = {9, 9, 9, 9};
    init_vec_bab950(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBAB950Bits[0], kVecBAB950Bits[1], kVecBAB950Bits[2], kVecBAB950Bits[3],
                    0x12dcc50ull, 0x3350e70ull, 0xbab950ull, 270u);
    } else {
        std::puts("vec_bab950: all checks passed");
    }
    return 0;
}
