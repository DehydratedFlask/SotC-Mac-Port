#include <cstdio>
#include <cstdlib>

#include "core/vec_b5ecf0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB5ECF0 g{1, 2, 3, 4};
    init_vec_b5ecf0(g);
    CHECK(g == kVecB5ECF0Bits);

    g = {9, 9, 9, 9};
    init_vec_b5ecf0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB5ECF0Bits[0], kVecB5ECF0Bits[1], kVecB5ECF0Bits[2], kVecB5ECF0Bits[3],
                    0x12d7cb0ull, 0x3350d10ull, 0xb5ecf0ull, 287u);
    } else {
        std::puts("vec_b5ecf0: all checks passed");
    }
    return 0;
}
