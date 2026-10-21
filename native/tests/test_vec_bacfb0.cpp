#include <cstdio>
#include <cstdlib>

#include "core/vec_bacfb0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBACFB0 g{1, 2, 3, 4};
    init_vec_bacfb0(g);
    CHECK(g == kVecBACFB0Bits);

    g = {9, 9, 9, 9};
    init_vec_bacfb0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBACFB0Bits[0], kVecBACFB0Bits[1], kVecBACFB0Bits[2], kVecBACFB0Bits[3],
                    0x12dd4a0ull, 0x3350ed0ull, 0xbacfb0ull, 266u);
    } else {
        std::puts("vec_bacfb0: all checks passed");
    }
    return 0;
}
