#include <cstdio>
#include <cstdlib>

#include "core/vec_cc07d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCc07d0 g{1, 2, 3, 4};
    init_vec_cc07d0(g);
    CHECK(g == kVecCc07d0Bits);

    g = {9, 9, 9, 9};
    init_vec_cc07d0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCc07d0Bits[0], kVecCc07d0Bits[1], kVecCc07d0Bits[2], kVecCc07d0Bits[3],
                    0x12f57d0ull, 0x3351bf0ull, 0xcc07d0ull, 165u);
    } else {
        std::puts("vec_cc07d0: all checks passed");
    }
    return 0;
}
