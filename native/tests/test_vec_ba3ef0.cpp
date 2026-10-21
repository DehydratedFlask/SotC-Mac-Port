#include <cstdio>
#include <cstdlib>

#include "core/vec_ba3ef0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBA3EF0 g{1, 2, 3, 4};
    init_vec_ba3ef0(g);
    CHECK(g == kVecBA3EF0Bits);

    g = {9, 9, 9, 9};
    init_vec_ba3ef0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBA3EF0Bits[0], kVecBA3EF0Bits[1], kVecBA3EF0Bits[2], kVecBA3EF0Bits[3],
                    0x12dc4d0ull, 0x3350e60ull, 0xba3ef0ull, 271u);
    } else {
        std::puts("vec_ba3ef0: all checks passed");
    }
    return 0;
}
