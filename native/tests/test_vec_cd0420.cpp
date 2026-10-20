#include <cstdio>
#include <cstdlib>

#include "core/vec_cd0420.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCd0420 g{1, 2, 3, 4};
    init_vec_cd0420(g);
    CHECK(g == kVecCd0420Bits);

    g = {9, 9, 9, 9};
    init_vec_cd0420(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCd0420Bits[0], kVecCd0420Bits[1], kVecCd0420Bits[2], kVecCd0420Bits[3],
                    0x12f6a20ull, 0x3351c50ull, 0xcd0420ull, 159u);
    } else {
        std::puts("vec_cd0420: all checks passed");
    }
    return 0;
}
