#include <cstdio>
#include <cstdlib>

#include "core/vec_c77070.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC77070 g{1, 2, 3, 4};
    init_vec_c77070(g);
    CHECK(g == kVecC77070Bits);

    g = {9, 9, 9, 9};
    init_vec_c77070(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC77070Bits[0], kVecC77070Bits[1], kVecC77070Bits[2], kVecC77070Bits[3],
                    0x12f1c00ull, 0x33519c0ull, 0xc77070ull, 183u);
    } else {
        std::puts("vec_c77070: all checks passed");
    }
    return 0;
}
