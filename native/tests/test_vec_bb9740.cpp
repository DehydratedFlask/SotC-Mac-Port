#include <cstdio>
#include <cstdlib>

#include "core/vec_bb9740.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBB9740 g{1, 2, 3, 4};
    init_vec_bb9740(g);
    CHECK(g == kVecBB9740Bits);

    g = {9, 9, 9, 9};
    init_vec_bb9740(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBB9740Bits[0], kVecBB9740Bits[1], kVecBB9740Bits[2], kVecBB9740Bits[3],
                    0x12dde70ull, 0x3350f20ull, 0xbb9740ull, 263u);
    } else {
        std::puts("vec_bb9740: all checks passed");
    }
    return 0;
}
