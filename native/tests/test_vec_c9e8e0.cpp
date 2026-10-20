#include <cstdio>
#include <cstdlib>

#include "core/vec_c9e8e0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC9e8e0 g{1, 2, 3, 4};
    init_vec_c9e8e0(g);
    CHECK(g == kVecC9e8e0Bits);

    g = {9, 9, 9, 9};
    init_vec_c9e8e0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC9e8e0Bits[0], kVecC9e8e0Bits[1], kVecC9e8e0Bits[2], kVecC9e8e0Bits[3],
                    0x12f3550ull, 0x3351aa0ull, 0xc9e8e0ull, 175u);
    } else {
        std::puts("vec_c9e8e0: all checks passed");
    }
    return 0;
}
