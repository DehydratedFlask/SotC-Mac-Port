#include <cstdio>
#include <cstdlib>

#include "core/vec_c47e00.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC47E00 g{1, 2, 3, 4};
    init_vec_c47e00(g);
    CHECK(g == kVecC47E00Bits);

    g = {9, 9, 9, 9};
    init_vec_c47e00(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC47E00Bits[0], kVecC47E00Bits[1], kVecC47E00Bits[2], kVecC47E00Bits[3],
                    0x12ed9a0ull, 0x3351640ull, 0xc47e00ull, 211u);
    } else {
        std::puts("vec_c47e00: all checks passed");
    }
    return 0;
}
