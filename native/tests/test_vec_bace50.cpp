#include <cstdio>
#include <cstdlib>

#include "core/vec_bace50.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBACE50 g{1, 2, 3, 4};
    init_vec_bace50(g);
    CHECK(g == kVecBACE50Bits);

    g = {9, 9, 9, 9};
    init_vec_bace50(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBACE50Bits[0], kVecBACE50Bits[1], kVecBACE50Bits[2], kVecBACE50Bits[3],
                    0x12dd180ull, 0x3350eb0ull, 0xbace50ull, 268u);
    } else {
        std::puts("vec_bace50: all checks passed");
    }
    return 0;
}
