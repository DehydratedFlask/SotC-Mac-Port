#include <cstdio>
#include <cstdlib>

#include "core/vec_bace80.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBACE80 g{1, 2, 3, 4};
    init_vec_bace80(g);
    CHECK(g == kVecBACE80Bits);

    g = {9, 9, 9, 9};
    init_vec_bace80(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBACE80Bits[0], kVecBACE80Bits[1], kVecBACE80Bits[2], kVecBACE80Bits[3],
                    0x12dd310ull, 0x3350ec0ull, 0xbace80ull, 267u);
    } else {
        std::puts("vec_bace80: all checks passed");
    }
    return 0;
}
