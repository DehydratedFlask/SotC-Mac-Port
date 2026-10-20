#include <cstdio>
#include <cstdlib>

#include "core/vec_ccd0f0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCcd0f0 g{1, 2, 3, 4};
    init_vec_ccd0f0(g);
    CHECK(g == kVecCcd0f0Bits);

    g = {9, 9, 9, 9};
    init_vec_ccd0f0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCcd0f0Bits[0], kVecCcd0f0Bits[1], kVecCcd0f0Bits[2], kVecCcd0f0Bits[3],
                    0x12f6560ull, 0x3351c30ull, 0xccd0f0ull, 161u);
    } else {
        std::puts("vec_ccd0f0: all checks passed");
    }
    return 0;
}
