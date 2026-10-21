#include <cstdio>
#include <cstdlib>

#include "core/vec_bbf580.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBBF580 g{1, 2, 3, 4};
    init_vec_bbf580(g);
    CHECK(g == kVecBBF580Bits);

    g = {9, 9, 9, 9};
    init_vec_bbf580(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBBF580Bits[0], kVecBBF580Bits[1], kVecBBF580Bits[2], kVecBBF580Bits[3],
                    0x12de530ull, 0x3350f50ull, 0xbbf580ull, 260u);
    } else {
        std::puts("vec_bbf580: all checks passed");
    }
    return 0;
}
