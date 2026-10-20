#include <cstdio>
#include <cstdlib>

#include "core/vec_d7c9d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD7c9d0 g{1, 2, 3, 4};
    init_vec_d7c9d0(g);
    CHECK(g == kVecD7c9d0Bits);

    g = {9, 9, 9, 9};
    init_vec_d7c9d0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD7c9d0Bits[0], kVecD7c9d0Bits[1], kVecD7c9d0Bits[2], kVecD7c9d0Bits[3],
                    0x1306eb0ull, 0x3352a20ull, 0xd7c9d0ull, 89u);
    } else {
        std::puts("vec_d7c9d0: all checks passed");
    }
    return 0;
}
