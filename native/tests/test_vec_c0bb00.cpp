#include <cstdio>
#include <cstdlib>

#include "core/vec_c0bb00.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC0BB00 g{1, 2, 3, 4};
    init_vec_c0bb00(g);
    CHECK(g == kVecC0BB00Bits);

    g = {9, 9, 9, 9};
    init_vec_c0bb00(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC0BB00Bits[0], kVecC0BB00Bits[1], kVecC0BB00Bits[2], kVecC0BB00Bits[3],
                    0x12e7830ull, 0x33512f0ull, 0xc0bb00ull, 232u);
    } else {
        std::puts("vec_c0bb00: all checks passed");
    }
    return 0;
}
