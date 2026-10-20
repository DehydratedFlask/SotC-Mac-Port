#include <cstdio>
#include <cstdlib>

#include "core/vec_dddc90.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDddc90 g{1, 2, 3, 4};
    init_vec_dddc90(g);
    CHECK(g == kVecDddc90Bits);

    g = {9, 9, 9, 9};
    init_vec_dddc90(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDddc90Bits[0], kVecDddc90Bits[1], kVecDddc90Bits[2], kVecDddc90Bits[3],
                    0x130f350ull, 0x3352ec0ull, 0xdddc90ull, 61u);
    } else {
        std::puts("vec_dddc90: all checks passed");
    }
    return 0;
}
