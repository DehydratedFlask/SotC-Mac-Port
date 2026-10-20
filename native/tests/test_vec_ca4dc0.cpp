#include <cstdio>
#include <cstdlib>

#include "core/vec_ca4dc0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCa4dc0 g{1, 2, 3, 4};
    init_vec_ca4dc0(g);
    CHECK(g == kVecCa4dc0Bits);

    g = {9, 9, 9, 9};
    init_vec_ca4dc0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCa4dc0Bits[0], kVecCa4dc0Bits[1], kVecCa4dc0Bits[2], kVecCa4dc0Bits[3],
                    0x12f3ac0ull, 0x3351b10ull, 0xca4dc0ull, 173u);
    } else {
        std::puts("vec_ca4dc0: all checks passed");
    }
    return 0;
}
