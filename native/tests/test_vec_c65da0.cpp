#include <cstdio>
#include <cstdlib>

#include "core/vec_c65da0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC65da0 g{1, 2, 3, 4};
    init_vec_c65da0(g);
    CHECK(g == kVecC65da0Bits);

    g = {9, 9, 9, 9};
    init_vec_c65da0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC65da0Bits[0], kVecC65da0Bits[1], kVecC65da0Bits[2], kVecC65da0Bits[3],
                    0x12efdd0ull, 0x3351730ull, 0xc65da0ull, 196u);
    } else {
        std::puts("vec_c65da0: all checks passed");
    }
    return 0;
}
