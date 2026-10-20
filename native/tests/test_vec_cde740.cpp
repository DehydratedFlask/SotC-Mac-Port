#include <cstdio>
#include <cstdlib>

#include "core/vec_cde740.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCde740 g{1, 2, 3, 4};
    init_vec_cde740(g);
    CHECK(g == kVecCde740Bits);

    g = {9, 9, 9, 9};
    init_vec_cde740(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCde740Bits[0], kVecCde740Bits[1], kVecCde740Bits[2], kVecCde740Bits[3],
                    0x12f77e0ull, 0x3351c80ull, 0xcde740ull, 156u);
    } else {
        std::puts("vec_cde740: all checks passed");
    }
    return 0;
}
