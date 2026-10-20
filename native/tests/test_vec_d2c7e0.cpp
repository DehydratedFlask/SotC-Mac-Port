#include <cstdio>
#include <cstdlib>

#include "core/vec_d2c7e0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD2c7e0 g{1, 2, 3, 4};
    init_vec_d2c7e0(g);
    CHECK(g == kVecD2c7e0Bits);

    g = {9, 9, 9, 9};
    init_vec_d2c7e0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD2c7e0Bits[0], kVecD2c7e0Bits[1], kVecD2c7e0Bits[2], kVecD2c7e0Bits[3],
                    0x12ff9e0ull, 0x3351ea0ull, 0xd2c7e0ull, 122u);
    } else {
        std::puts("vec_d2c7e0: all checks passed");
    }
    return 0;
}
