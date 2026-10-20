#include <cstdio>
#include <cstdlib>

#include "core/vec_c0d720.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC0D720 g{1, 2, 3, 4};
    init_vec_c0d720(g);
    CHECK(g == kVecC0D720Bits);

    g = {9, 9, 9, 9};
    init_vec_c0d720(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC0D720Bits[0], kVecC0D720Bits[1], kVecC0D720Bits[2], kVecC0D720Bits[3],
                    0x12e8260ull, 0x33513c0ull, 0xc0d720ull, 228u);
    } else {
        std::puts("vec_c0d720: all checks passed");
    }
    return 0;
}
