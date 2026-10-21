#include <cstdio>
#include <cstdlib>

#include "core/vec_bc0670.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBC0670 g{1, 2, 3, 4};
    init_vec_bc0670(g);
    CHECK(g == kVecBC0670Bits);

    g = {9, 9, 9, 9};
    init_vec_bc0670(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBC0670Bits[0], kVecBC0670Bits[1], kVecBC0670Bits[2], kVecBC0670Bits[3],
                    0x12de6c0ull, 0x3350f60ull, 0xbc0670ull, 259u);
    } else {
        std::puts("vec_bc0670: all checks passed");
    }
    return 0;
}
