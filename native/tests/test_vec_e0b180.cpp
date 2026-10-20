#include <cstdio>
#include <cstdlib>

#include "core/vec_e0b180.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecE0b180 g{1, 2, 3, 4};
    init_vec_e0b180(g);
    CHECK(g == kVecE0b180Bits);

    g = {9, 9, 9, 9};
    init_vec_e0b180(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecE0b180Bits[0], kVecE0b180Bits[1], kVecE0b180Bits[2], kVecE0b180Bits[3],
                    0x13141b0ull, 0x3353390ull, 0xe0b180ull, 49u);
    } else {
        std::puts("vec_e0b180: all checks passed");
    }
    return 0;
}
