#include <cstdio>
#include <cstdlib>

#include "core/vec_c786c0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC786c0 g{1, 2, 3, 4};
    init_vec_c786c0(g);
    CHECK(g == kVecC786c0Bits);

    g = {9, 9, 9, 9};
    init_vec_c786c0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC786c0Bits[0], kVecC786c0Bits[1], kVecC786c0Bits[2], kVecC786c0Bits[3],
                    0x12f1ee0ull, 0x33519d0ull, 0xc786c0ull, 182u);
    } else {
        std::puts("vec_c786c0: all checks passed");
    }
    return 0;
}
