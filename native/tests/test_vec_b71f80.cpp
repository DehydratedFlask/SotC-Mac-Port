#include <cstdio>
#include <cstdlib>

#include "core/vec_b71f80.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB71F80 g{1, 2, 3, 4};
    init_vec_b71f80(g);
    CHECK(g == kVecB71F80Bits);

    g = {9, 9, 9, 9};
    init_vec_b71f80(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB71F80Bits[0], kVecB71F80Bits[1], kVecB71F80Bits[2], kVecB71F80Bits[3],
                    0x12d8b10ull, 0x3350da0ull, 0xb71f80ull, 283u);
    } else {
        std::puts("vec_b71f80: all checks passed");
    }
    return 0;
}
