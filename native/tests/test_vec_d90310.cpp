#include <cstdio>
#include <cstdlib>

#include "core/vec_d90310.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD90310 g{1, 2, 3, 4};
    init_vec_d90310(g);
    CHECK(g == kVecD90310Bits);

    g = {9, 9, 9, 9};
    init_vec_d90310(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD90310Bits[0], kVecD90310Bits[1], kVecD90310Bits[2], kVecD90310Bits[3],
                    0x130b1a0ull, 0x3352c10ull, 0xd90310ull, 79u);
    } else {
        std::puts("vec_d90310: all checks passed");
    }
    return 0;
}
