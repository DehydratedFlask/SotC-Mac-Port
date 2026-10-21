#include <cstdio>
#include <cstdlib>

#include "core/vec_b8d170.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB8D170 g{1, 2, 3, 4};
    init_vec_b8d170(g);
    CHECK(g == kVecB8D170Bits);

    g = {9, 9, 9, 9};
    init_vec_b8d170(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB8D170Bits[0], kVecB8D170Bits[1], kVecB8D170Bits[2], kVecB8D170Bits[3],
                    0x12db4a0ull, 0x3350e40ull, 0xb8d170ull, 273u);
    } else {
        std::puts("vec_b8d170: all checks passed");
    }
    return 0;
}
