#include <cstdio>
#include <cstdlib>

#include "core/vec_b4ef70.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB4EF70 g{1, 2, 3, 4};
    init_vec_b4ef70(g);
    CHECK(g == kVecB4EF70Bits);

    g = {9, 9, 9, 9};
    init_vec_b4ef70(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB4EF70Bits[0], kVecB4EF70Bits[1], kVecB4EF70Bits[2], kVecB4EF70Bits[3],
                    0x12d65d0ull, 0x3350cd0ull, 0xb4ef70ull, 291u);
    } else {
        std::puts("vec_b4ef70: all checks passed");
    }
    return 0;
}
