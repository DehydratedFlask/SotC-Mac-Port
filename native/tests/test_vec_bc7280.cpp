#include <cstdio>
#include <cstdlib>

#include "core/vec_bc7280.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBC7280 g{1, 2, 3, 4};
    init_vec_bc7280(g);
    CHECK(g == kVecBC7280Bits);

    g = {9, 9, 9, 9};
    init_vec_bc7280(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBC7280Bits[0], kVecBC7280Bits[1], kVecBC7280Bits[2], kVecBC7280Bits[3],
                    0x12df790ull, 0x3350ff0ull, 0xbc7280ull, 255u);
    } else {
        std::puts("vec_bc7280: all checks passed");
    }
    return 0;
}
