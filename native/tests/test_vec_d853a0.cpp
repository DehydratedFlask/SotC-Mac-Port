#include <cstdio>
#include <cstdlib>

#include "core/vec_d853a0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD853a0 g{1, 2, 3, 4};
    init_vec_d853a0(g);
    CHECK(g == kVecD853a0Bits);

    g = {9, 9, 9, 9};
    init_vec_d853a0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD853a0Bits[0], kVecD853a0Bits[1], kVecD853a0Bits[2], kVecD853a0Bits[3],
                    0x130a720ull, 0x3352a80ull, 0xd853a0ull, 83u);
    } else {
        std::puts("vec_d853a0: all checks passed");
    }
    return 0;
}
