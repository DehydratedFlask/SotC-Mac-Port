#include <cstdio>
#include <cstdlib>

#include "core/vec_d13300.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD13300 g{1, 2, 3, 4};
    init_vec_d13300(g);
    CHECK(g == kVecD13300Bits);

    g = {9, 9, 9, 9};
    init_vec_d13300(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD13300Bits[0], kVecD13300Bits[1], kVecD13300Bits[2], kVecD13300Bits[3],
                    0x12fde60ull, 0x3351e40ull, 0xd13300ull, 128u);
    } else {
        std::puts("vec_d13300: all checks passed");
    }
    return 0;
}
