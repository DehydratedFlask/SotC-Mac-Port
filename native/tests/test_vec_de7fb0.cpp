#include <cstdio>
#include <cstdlib>

#include "core/vec_de7fb0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDe7fb0 g{1, 2, 3, 4};
    init_vec_de7fb0(g);
    CHECK(g == kVecDe7fb0Bits);

    g = {9, 9, 9, 9};
    init_vec_de7fb0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDe7fb0Bits[0], kVecDe7fb0Bits[1], kVecDe7fb0Bits[2], kVecDe7fb0Bits[3],
                    0x1310c90ull, 0x3353320ull, 0xde7fb0ull, 56u);
    } else {
        std::puts("vec_de7fb0: all checks passed");
    }
    return 0;
}
