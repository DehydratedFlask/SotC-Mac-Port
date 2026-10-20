#include <cstdio>
#include <cstdlib>

#include "core/vec_d19930.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD19930 g{1, 2, 3, 4};
    init_vec_d19930(g);
    CHECK(g == kVecD19930Bits);

    g = {9, 9, 9, 9};
    init_vec_d19930(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD19930Bits[0], kVecD19930Bits[1], kVecD19930Bits[2], kVecD19930Bits[3],
                    0x12fe8e0ull, 0x3351e60ull, 0xd19930ull, 126u);
    } else {
        std::puts("vec_d19930: all checks passed");
    }
    return 0;
}
