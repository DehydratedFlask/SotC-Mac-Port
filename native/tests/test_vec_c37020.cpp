#include <cstdio>
#include <cstdlib>

#include "core/vec_c37020.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC37020 g{1, 2, 3, 4};
    init_vec_c37020(g);
    CHECK(g == kVecC37020Bits);

    g = {9, 9, 9, 9};
    init_vec_c37020(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC37020Bits[0], kVecC37020Bits[1], kVecC37020Bits[2], kVecC37020Bits[3],
                    0x12ec290ull, 0x33515e0ull, 0xc37020ull, 217u);
    } else {
        std::puts("vec_c37020: all checks passed");
    }
    return 0;
}
