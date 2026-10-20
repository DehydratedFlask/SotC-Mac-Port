#include <cstdio>
#include <cstdlib>

#include "core/vec_dbb280.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDbb280 g{1, 2, 3, 4};
    init_vec_dbb280(g);
    CHECK(g == kVecDbb280Bits);

    g = {9, 9, 9, 9};
    init_vec_dbb280(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDbb280Bits[0], kVecDbb280Bits[1], kVecDbb280Bits[2], kVecDbb280Bits[3],
                    0x130dbb0ull, 0x3352c90ull, 0xdbb280ull, 71u);
    } else {
        std::puts("vec_dbb280: all checks passed");
    }
    return 0;
}
