#include <cstdio>
#include <cstdlib>

#include "core/vec_bd3190.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBD3190 g{1, 2, 3, 4};
    init_vec_bd3190(g);
    CHECK(g == kVecBD3190Bits);

    g = {9, 9, 9, 9};
    init_vec_bd3190(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBD3190Bits[0], kVecBD3190Bits[1], kVecBD3190Bits[2], kVecBD3190Bits[3],
                    0x12e12c0ull, 0x3351170ull, 0xbd3190ull, 247u);
    } else {
        std::puts("vec_bd3190: all checks passed");
    }
    return 0;
}
