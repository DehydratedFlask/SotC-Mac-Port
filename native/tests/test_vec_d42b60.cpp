#include <cstdio>
#include <cstdlib>

#include "core/vec_d42b60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD42b60 g{1, 2, 3, 4};
    init_vec_d42b60(g);
    CHECK(g == kVecD42b60Bits);

    g = {9, 9, 9, 9};
    init_vec_d42b60(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD42b60Bits[0], kVecD42b60Bits[1], kVecD42b60Bits[2], kVecD42b60Bits[3],
                    0x1302650ull, 0x33520e0ull, 0xd42b60ull, 108u);
    } else {
        std::puts("vec_d42b60: all checks passed");
    }
    return 0;
}
