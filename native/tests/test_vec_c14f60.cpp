#include <cstdio>
#include <cstdlib>

#include "core/vec_c14f60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC14F60 g{1, 2, 3, 4};
    init_vec_c14f60(g);
    CHECK(g == kVecC14F60Bits);

    g = {9, 9, 9, 9};
    init_vec_c14f60(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC14F60Bits[0], kVecC14F60Bits[1], kVecC14F60Bits[2], kVecC14F60Bits[3],
                    0x12e84f0ull, 0x33513d0ull, 0xc14f60ull, 227u);
    } else {
        std::puts("vec_c14f60: all checks passed");
    }
    return 0;
}
