#include <cstdio>
#include <cstdlib>

#include "core/vec_bb7990.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBB7990 g{1, 2, 3, 4};
    init_vec_bb7990(g);
    CHECK(g == kVecBB7990Bits);

    g = {9, 9, 9, 9};
    init_vec_bb7990(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBB7990Bits[0], kVecBB7990Bits[1], kVecBB7990Bits[2], kVecBB7990Bits[3],
                    0x12dd920ull, 0x3350ee0ull, 0xbb7990ull, 265u);
    } else {
        std::puts("vec_bb7990: all checks passed");
    }
    return 0;
}
