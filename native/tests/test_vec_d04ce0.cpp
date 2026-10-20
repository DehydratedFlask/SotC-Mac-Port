#include <cstdio>
#include <cstdlib>

#include "core/vec_d04ce0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD04ce0 g{1, 2, 3, 4};
    init_vec_d04ce0(g);
    CHECK(g == kVecD04ce0Bits);

    g = {9, 9, 9, 9};
    init_vec_d04ce0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD04ce0Bits[0], kVecD04ce0Bits[1], kVecD04ce0Bits[2], kVecD04ce0Bits[3],
                    0x12fc350ull, 0x3351dc0ull, 0xd04ce0ull, 136u);
    } else {
        std::puts("vec_d04ce0: all checks passed");
    }
    return 0;
}
