#include <cstdio>
#include <cstdlib>

#include "core/vec_d94cb0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD94cb0 g{1, 2, 3, 4};
    init_vec_d94cb0(g);
    CHECK(g == kVecD94cb0Bits);

    g = {9, 9, 9, 9};
    init_vec_d94cb0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD94cb0Bits[0], kVecD94cb0Bits[1], kVecD94cb0Bits[2], kVecD94cb0Bits[3],
                    0x130b660ull, 0x3352c20ull, 0xd94cb0ull, 78u);
    } else {
        std::puts("vec_d94cb0: all checks passed");
    }
    return 0;
}
