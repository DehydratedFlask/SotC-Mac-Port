#include <cstdio>
#include <cstdlib>

#include "core/vec_d6dbf0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD6dbf0 g{1, 2, 3, 4};
    init_vec_d6dbf0(g);
    CHECK(g == kVecD6dbf0Bits);

    g = {9, 9, 9, 9};
    init_vec_d6dbf0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD6dbf0Bits[0], kVecD6dbf0Bits[1], kVecD6dbf0Bits[2], kVecD6dbf0Bits[3],
                    0x13052d0ull, 0x33529a0ull, 0xd6dbf0ull, 97u);
    } else {
        std::puts("vec_d6dbf0: all checks passed");
    }
    return 0;
}
