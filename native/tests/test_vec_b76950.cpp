#include <cstdio>
#include <cstdlib>

#include "core/vec_b76950.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB76950 g{1, 2, 3, 4};
    init_vec_b76950(g);
    CHECK(g == kVecB76950Bits);

    g = {9, 9, 9, 9};
    init_vec_b76950(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB76950Bits[0], kVecB76950Bits[1], kVecB76950Bits[2], kVecB76950Bits[3],
                    0x12d9350ull, 0x3350dd0ull, 0xb76950ull, 280u);
    } else {
        std::puts("vec_b76950: all checks passed");
    }
    return 0;
}
