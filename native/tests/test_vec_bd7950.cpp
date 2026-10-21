#include <cstdio>
#include <cstdlib>

#include "core/vec_bd7950.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBD7950 g{1, 2, 3, 4};
    init_vec_bd7950(g);
    CHECK(g == kVecBD7950Bits);

    g = {9, 9, 9, 9};
    init_vec_bd7950(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBD7950Bits[0], kVecBD7950Bits[1], kVecBD7950Bits[2], kVecBD7950Bits[3],
                    0x12e1e30ull, 0x33511b0ull, 0xbd7950ull, 243u);
    } else {
        std::puts("vec_bd7950: all checks passed");
    }
    return 0;
}
