#include <cstdio>
#include <cstdlib>

#include "core/vec_c61670.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC61670 g{1, 2, 3, 4};
    init_vec_c61670(g);
    CHECK(g == kVecC61670Bits);

    g = {9, 9, 9, 9};
    init_vec_c61670(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC61670Bits[0], kVecC61670Bits[1], kVecC61670Bits[2], kVecC61670Bits[3],
                    0x12ef8c0ull, 0x3351710ull, 0xc61670ull, 198u);
    } else {
        std::puts("vec_c61670: all checks passed");
    }
    return 0;
}
