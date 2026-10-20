#include <cstdio>
#include <cstdlib>

#include "core/vec_d6d890.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD6d890 g{1, 2, 3, 4};
    init_vec_d6d890(g);
    CHECK(g == kVecD6d890Bits);

    g = {9, 9, 9, 9};
    init_vec_d6d890(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD6d890Bits[0], kVecD6d890Bits[1], kVecD6d890Bits[2], kVecD6d890Bits[3],
                    0x13050b0ull, 0x3352990ull, 0xd6d890ull, 98u);
    } else {
        std::puts("vec_d6d890: all checks passed");
    }
    return 0;
}
