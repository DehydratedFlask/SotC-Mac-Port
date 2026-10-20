#include <cstdio>
#include <cstdlib>

#include "core/vec_c7de60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC7de60 g{1, 2, 3, 4};
    init_vec_c7de60(g);
    CHECK(g == kVecC7de60Bits);

    g = {9, 9, 9, 9};
    init_vec_c7de60(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC7de60Bits[0], kVecC7de60Bits[1], kVecC7de60Bits[2], kVecC7de60Bits[3],
                    0x12f2520ull, 0x33519f0ull, 0xc7de60ull, 180u);
    } else {
        std::puts("vec_c7de60: all checks passed");
    }
    return 0;
}
