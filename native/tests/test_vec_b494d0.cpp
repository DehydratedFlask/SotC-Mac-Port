#include <cstdio>
#include <cstdlib>

#include "core/vec_b494d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB494D0 g{1, 2, 3, 4};
    init_vec_b494d0(g);
    CHECK(g == kVecB494D0Bits);

    g = {9, 9, 9, 9};
    init_vec_b494d0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB494D0Bits[0], kVecB494D0Bits[1], kVecB494D0Bits[2], kVecB494D0Bits[3],
                    0x12d5eb0ull, 0x3350cc0ull, 0xb494d0ull, 292u);
    } else {
        std::puts("vec_b494d0: all checks passed");
    }
    return 0;
}
