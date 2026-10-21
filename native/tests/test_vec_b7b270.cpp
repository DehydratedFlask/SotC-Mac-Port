#include <cstdio>
#include <cstdlib>

#include "core/vec_b7b270.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB7B270 g{1, 2, 3, 4};
    init_vec_b7b270(g);
    CHECK(g == kVecB7B270Bits);

    g = {9, 9, 9, 9};
    init_vec_b7b270(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB7B270Bits[0], kVecB7B270Bits[1], kVecB7B270Bits[2], kVecB7B270Bits[3],
                    0x12d9b30ull, 0x3350e10ull, 0xb7b270ull, 276u);
    } else {
        std::puts("vec_b7b270: all checks passed");
    }
    return 0;
}
