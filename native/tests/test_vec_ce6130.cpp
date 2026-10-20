#include <cstdio>
#include <cstdlib>

#include "core/vec_ce6130.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCe6130 g{1, 2, 3, 4};
    init_vec_ce6130(g);
    CHECK(g == kVecCe6130Bits);

    g = {9, 9, 9, 9};
    init_vec_ce6130(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCe6130Bits[0], kVecCe6130Bits[1], kVecCe6130Bits[2], kVecCe6130Bits[3],
                    0x12f8560ull, 0x3351cc0ull, 0xce6130ull, 152u);
    } else {
        std::puts("vec_ce6130: all checks passed");
    }
    return 0;
}
