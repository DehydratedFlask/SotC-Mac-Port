#include <cstdio>
#include <cstdlib>

#include "core/vec_d3b790.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD3b790 g{1, 2, 3, 4};
    init_vec_d3b790(g);
    CHECK(g == kVecD3b790Bits);

    g = {9, 9, 9, 9};
    init_vec_d3b790(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD3b790Bits[0], kVecD3b790Bits[1], kVecD3b790Bits[2], kVecD3b790Bits[3],
                    0x1301a50ull, 0x33520a0ull, 0xd3b790ull, 112u);
    } else {
        std::puts("vec_d3b790: all checks passed");
    }
    return 0;
}
