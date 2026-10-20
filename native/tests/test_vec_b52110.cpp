#include <cstdio>
#include <cstdlib>

#include "core/vec_b52110.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB52110 g{1, 2, 3, 4};
    init_vec_b52110(g);
    CHECK(g == kVecB52110Bits);

    g = {9, 9, 9, 9};
    init_vec_b52110(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB52110Bits[0], kVecB52110Bits[1], kVecB52110Bits[2], kVecB52110Bits[3],
                    0x12d6b20ull, 0x3350ce0ull, 0xb52110ull, 290u);
    } else {
        std::puts("vec_b52110: all checks passed");
    }
    return 0;
}
