#include <cstdio>
#include <cstdlib>

#include "core/vec_bbd5e0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBBD5E0 g{1, 2, 3, 4};
    init_vec_bbd5e0(g);
    CHECK(g == kVecBBD5E0Bits);

    g = {9, 9, 9, 9};
    init_vec_bbd5e0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBBD5E0Bits[0], kVecBBD5E0Bits[1], kVecBBD5E0Bits[2], kVecBBD5E0Bits[3],
                    0x12de320ull, 0x3350f40ull, 0xbbd5e0ull, 261u);
    } else {
        std::puts("vec_bbd5e0: all checks passed");
    }
    return 0;
}
