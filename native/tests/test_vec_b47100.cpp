#include <cstdio>
#include <cstdlib>

#include "core/vec_b47100.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB47100 g{1, 2, 3, 4};
    init_vec_b47100(g);
    CHECK(g == kVecB47100Bits);

    g = {9, 9, 9, 9};
    init_vec_b47100(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB47100Bits[0], kVecB47100Bits[1], kVecB47100Bits[2], kVecB47100Bits[3],
                    0x12d5af0ull, 0x3350cb0ull, 0xb47100ull, 293u);
    } else {
        std::puts("vec_b47100: all checks passed");
    }
    return 0;
}
