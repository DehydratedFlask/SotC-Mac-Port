#include <cstdio>
#include <cstdlib>

#include "core/vec_b57360.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB57360 g{1, 2, 3, 4};
    init_vec_b57360(g);
    CHECK(g == kVecB57360Bits);

    g = {9, 9, 9, 9};
    init_vec_b57360(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB57360Bits[0], kVecB57360Bits[1], kVecB57360Bits[2], kVecB57360Bits[3],
                    0x12d76c0ull, 0x3350cf0ull, 0xb57360ull, 289u);
    } else {
        std::puts("vec_b57360: all checks passed");
    }
    return 0;
}
