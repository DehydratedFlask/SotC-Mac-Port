#include <cstdio>
#include <cstdlib>

#include "core/vec_bc07f0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBC07F0 g{1, 2, 3, 4};
    init_vec_bc07f0(g);
    CHECK(g == kVecBC07F0Bits);

    g = {9, 9, 9, 9};
    init_vec_bc07f0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBC07F0Bits[0], kVecBC07F0Bits[1], kVecBC07F0Bits[2], kVecBC07F0Bits[3],
                    0x12ded00ull, 0x3350fc0ull, 0xbc07f0ull, 258u);
    } else {
        std::puts("vec_bc07f0: all checks passed");
    }
    return 0;
}
