#include <cstdio>
#include <cstdlib>

#include "core/vec_c5c320.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC5c320 g{1, 2, 3, 4};
    init_vec_c5c320(g);
    CHECK(g == kVecC5c320Bits);

    g = {9, 9, 9, 9};
    init_vec_c5c320(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC5c320Bits[0], kVecC5c320Bits[1], kVecC5c320Bits[2], kVecC5c320Bits[3],
                    0x12ef3a0ull, 0x33516f0ull, 0xc5c320ull, 200u);
    } else {
        std::puts("vec_c5c320: all checks passed");
    }
    return 0;
}
