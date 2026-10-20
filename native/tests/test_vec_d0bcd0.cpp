#include <cstdio>
#include <cstdlib>

#include "core/vec_d0bcd0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD0bcd0 g{1, 2, 3, 4};
    init_vec_d0bcd0(g);
    CHECK(g == kVecD0bcd0Bits);

    g = {9, 9, 9, 9};
    init_vec_d0bcd0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD0bcd0Bits[0], kVecD0bcd0Bits[1], kVecD0bcd0Bits[2], kVecD0bcd0Bits[3],
                    19912816ull, 53812768ull, 0xd0bcd0ull, 126u);
    } else {
        std::puts("vec_d0bcd0: all checks passed");
    }
    return 0;
}
