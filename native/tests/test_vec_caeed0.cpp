#include <cstdio>
#include <cstdlib>

#include "core/vec_caeed0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCaeed0 g{1, 2, 3, 4};
    init_vec_caeed0(g);
    CHECK(g == kVecCaeed0Bits);

    g = {9, 9, 9, 9};
    init_vec_caeed0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCaeed0Bits[0], kVecCaeed0Bits[1], kVecCaeed0Bits[2], kVecCaeed0Bits[3],
                    0x12f4660ull, 0x3351bc0ull, 0xcaeed0ull, 168u);
    } else {
        std::puts("vec_caeed0: all checks passed");
    }
    return 0;
}
