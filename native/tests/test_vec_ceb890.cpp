#include <cstdio>
#include <cstdlib>

#include "core/vec_ceb890.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCeb890 g{1, 2, 3, 4};
    init_vec_ceb890(g);
    CHECK(g == kVecCeb890Bits);

    g = {9, 9, 9, 9};
    init_vec_ceb890(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCeb890Bits[0], kVecCeb890Bits[1], kVecCeb890Bits[2], kVecCeb890Bits[3],
                    0x12f9b60ull, 0x3351d60ull, 0xceb890ull, 142u);
    } else {
        std::puts("vec_ceb890: all checks passed");
    }
    return 0;
}
