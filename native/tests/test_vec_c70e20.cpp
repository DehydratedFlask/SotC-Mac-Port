#include <cstdio>
#include <cstdlib>

#include "core/vec_c70e20.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC70e20 g{1, 2, 3, 4};
    init_vec_c70e20(g);
    CHECK(g == kVecC70e20Bits);

    g = {9, 9, 9, 9};
    init_vec_c70e20(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC70e20Bits[0], kVecC70e20Bits[1], kVecC70e20Bits[2], kVecC70e20Bits[3],
                    0x12f16a0ull, 0x33519a0ull, 0xc70e20ull, 185u);
    } else {
        std::puts("vec_c70e20: all checks passed");
    }
    return 0;
}
