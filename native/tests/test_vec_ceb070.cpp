#include <cstdio>
#include <cstdlib>

#include "core/vec_ceb070.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCeb070 g{1, 2, 3, 4};
    init_vec_ceb070(g);
    CHECK(g == kVecCeb070Bits);

    g = {9, 9, 9, 9};
    init_vec_ceb070(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCeb070Bits[0], kVecCeb070Bits[1], kVecCeb070Bits[2], kVecCeb070Bits[3],
                    0x12f95c0ull, 0x3351d30ull, 0xceb070ull, 87u);
    } else {
        std::puts("vec_ceb070: all checks passed");
    }
    return 0;
}
