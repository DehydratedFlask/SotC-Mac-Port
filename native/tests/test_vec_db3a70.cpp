#include <cstdio>
#include <cstdlib>

#include "core/vec_db3a70.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDb3a70 g{1, 2, 3, 4};
    init_vec_db3a70(g);
    CHECK(g == kVecDb3a70Bits);

    g = {9, 9, 9, 9};
    init_vec_db3a70(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDb3a70Bits[0], kVecDb3a70Bits[1], kVecDb3a70Bits[2], kVecDb3a70Bits[3],
                    0x130d1e0ull, 0x3352c50ull, 0xdb3a70ull, 75u);
    } else {
        std::puts("vec_db3a70: all checks passed");
    }
    return 0;
}
