#include <cstdio>
#include <cstdlib>

#include "core/vec_c20710.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC20710 g{1, 2, 3, 4};
    init_vec_c20710(g);
    CHECK(g == kVecC20710Bits);

    g = {9, 9, 9, 9};
    init_vec_c20710(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC20710Bits[0], kVecC20710Bits[1], kVecC20710Bits[2], kVecC20710Bits[3],
                    0x12e9d10ull, 0x3351560ull, 0xc20710ull, 221u);
    } else {
        std::puts("vec_c20710: all checks passed");
    }
    return 0;
}
