#include <cstdio>
#include <cstdlib>

#include "core/vec_b950e0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB950E0 g{1, 2, 3, 4};
    init_vec_b950e0(g);
    CHECK(g == kVecB950E0Bits);

    g = {9, 9, 9, 9};
    init_vec_b950e0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB950E0Bits[0], kVecB950E0Bits[1], kVecB950E0Bits[2], kVecB950E0Bits[3],
                    0x12dbb70ull, 0x3350e50ull, 0xb950e0ull, 272u);
    } else {
        std::puts("vec_b950e0: all checks passed");
    }
    return 0;
}
