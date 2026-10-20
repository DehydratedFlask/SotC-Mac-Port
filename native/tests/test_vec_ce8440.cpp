#include <cstdio>
#include <cstdlib>

#include "core/vec_ce8440.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCe8440 g{1, 2, 3, 4};
    init_vec_ce8440(g);
    CHECK(g == kVecCe8440Bits);

    g = {9, 9, 9, 9};
    init_vec_ce8440(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCe8440Bits[0], kVecCe8440Bits[1], kVecCe8440Bits[2], kVecCe8440Bits[3],
                    0x12f9100ull, 0x3351d10ull, 0xce8440ull, 147u);
    } else {
        std::puts("vec_ce8440: all checks passed");
    }
    return 0;
}
