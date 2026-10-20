#include <cstdio>
#include <cstdlib>

#include "core/vec_c3eaa0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC3EAA0 g{1, 2, 3, 4};
    init_vec_c3eaa0(g);
    CHECK(g == kVecC3EAA0Bits);

    g = {9, 9, 9, 9};
    init_vec_c3eaa0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC3EAA0Bits[0], kVecC3EAA0Bits[1], kVecC3EAA0Bits[2], kVecC3EAA0Bits[3],
                    0x12ecc40ull, 0x3351600ull, 0xc3eaa0ull, 215u);
    } else {
        std::puts("vec_c3eaa0: all checks passed");
    }
    return 0;
}
