#include <cstdio>
#include <cstdlib>

#include "core/vec_e05320.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecE05320 g{1, 2, 3, 4};
    init_vec_e05320(g);
    CHECK(g == kVecE05320Bits);

    g = {9, 9, 9, 9};
    init_vec_e05320(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecE05320Bits[0], kVecE05320Bits[1], kVecE05320Bits[2], kVecE05320Bits[3],
                    0x13132d0ull, 0x3353360ull, 0xe05320ull, 52u);
    } else {
        std::puts("vec_e05320: all checks passed");
    }
    return 0;
}
