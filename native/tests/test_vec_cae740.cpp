#include <cstdio>
#include <cstdlib>

#include "core/vec_cae740.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCae740 g{1, 2, 3, 4};
    init_vec_cae740(g);
    CHECK(g == kVecCae740Bits);

    g = {9, 9, 9, 9};
    init_vec_cae740(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCae740Bits[0], kVecCae740Bits[1], kVecCae740Bits[2], kVecCae740Bits[3],
                    0x12f4490ull, 0x3351bb0ull, 0xcae740ull, 169u);
    } else {
        std::puts("vec_cae740: all checks passed");
    }
    return 0;
}
