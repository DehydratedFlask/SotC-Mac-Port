#include <cstdio>
#include <cstdlib>

#include "core/vec_c451d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC451D0 g{1, 2, 3, 4};
    init_vec_c451d0(g);
    CHECK(g == kVecC451D0Bits);

    g = {9, 9, 9, 9};
    init_vec_c451d0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC451D0Bits[0], kVecC451D0Bits[1], kVecC451D0Bits[2], kVecC451D0Bits[3],
                    0x12ed6e0ull, 0x3351630ull, 0xc451d0ull, 212u);
    } else {
        std::puts("vec_c451d0: all checks passed");
    }
    return 0;
}
