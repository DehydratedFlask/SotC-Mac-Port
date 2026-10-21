#include <cstdio>
#include <cstdlib>

#include "core/vec_bccc00.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBCCC00 g{1, 2, 3, 4};
    init_vec_bccc00(g);
    CHECK(g == kVecBCCC00Bits);

    g = {9, 9, 9, 9};
    init_vec_bccc00(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBCCC00Bits[0], kVecBCCC00Bits[1], kVecBCCC00Bits[2], kVecBCCC00Bits[3],
                    0x12e0680ull, 0x3351140ull, 0xbccc00ull, 250u);
    } else {
        std::puts("vec_bccc00: all checks passed");
    }
    return 0;
}
