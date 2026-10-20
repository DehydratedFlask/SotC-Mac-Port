#include <cstdio>
#include <cstdlib>

#include "core/vec_cd4d60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCd4d60 g{1, 2, 3, 4};
    init_vec_cd4d60(g);
    CHECK(g == kVecCd4d60Bits);

    g = {9, 9, 9, 9};
    init_vec_cd4d60(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCd4d60Bits[0], kVecCd4d60Bits[1], kVecCd4d60Bits[2], kVecCd4d60Bits[3],
                    0x12f6e70ull, 0x3351c60ull, 0xcd4d60ull, 158u);
    } else {
        std::puts("vec_cd4d60: all checks passed");
    }
    return 0;
}
