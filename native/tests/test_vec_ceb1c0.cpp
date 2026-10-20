#include <cstdio>
#include <cstdlib>

#include "core/vec_ceb1c0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCeb1c0 g{1, 2, 3, 4};
    init_vec_ceb1c0(g);
    CHECK(g == kVecCeb1c0Bits);

    g = {9, 9, 9, 9};
    init_vec_ceb1c0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCeb1c0Bits[0], kVecCeb1c0Bits[1], kVecCeb1c0Bits[2], kVecCeb1c0Bits[3],
                    0x12f97d0ull, 0x3351d40ull, 0xceb1c0ull, 144u);
    } else {
        std::puts("vec_ceb1c0: all checks passed");
    }
    return 0;
}
