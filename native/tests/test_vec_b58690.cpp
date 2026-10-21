#include <cstdio>
#include <cstdlib>

#include "core/vec_b58690.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB58690 g{1, 2, 3, 4};
    init_vec_b58690(g);
    CHECK(g == kVecB58690Bits);

    g = {9, 9, 9, 9};
    init_vec_b58690(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB58690Bits[0], kVecB58690Bits[1], kVecB58690Bits[2], kVecB58690Bits[3],
                    0x12d78d0ull, 0x3350d00ull, 0xb58690ull, 288u);
    } else {
        std::puts("vec_b58690: all checks passed");
    }
    return 0;
}
