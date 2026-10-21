#include <cstdio>
#include <cstdlib>

#include "core/vec_b77740.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB77740 g{1, 2, 3, 4};
    init_vec_b77740(g);
    CHECK(g == kVecB77740Bits);

    g = {9, 9, 9, 9};
    init_vec_b77740(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB77740Bits[0], kVecB77740Bits[1], kVecB77740Bits[2], kVecB77740Bits[3],
                    0x12d95b0ull, 0x3350de0ull, 0xb77740ull, 279u);
    } else {
        std::puts("vec_b77740: all checks passed");
    }
    return 0;
}
