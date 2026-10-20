#include <cstdio>
#include <cstdlib>

#include "core/vec_d7b930.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD7b930 g{1, 2, 3, 4};
    init_vec_d7b930(g);
    CHECK(g == kVecD7b930Bits);

    g = {9, 9, 9, 9};
    init_vec_d7b930(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD7b930Bits[0], kVecD7b930Bits[1], kVecD7b930Bits[2], kVecD7b930Bits[3],
                    0x1306100ull, 0x33529d0ull, 0xd7b930ull, 94u);
    } else {
        std::puts("vec_d7b930: all checks passed");
    }
    return 0;
}
