#include <cstdio>
#include <cstdlib>

#include "core/vec_bacb30.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBACB30 g{1, 2, 3, 4};
    init_vec_bacb30(g);
    CHECK(g == kVecBACB30Bits);

    g = {9, 9, 9, 9};
    init_vec_bacb30(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBACB30Bits[0], kVecBACB30Bits[1], kVecBACB30Bits[2], kVecBACB30Bits[3],
                    0x12dcff0ull, 0x3350e80ull, 0xbacb30ull, 269u);
    } else {
        std::puts("vec_bacb30: all checks passed");
    }
    return 0;
}
