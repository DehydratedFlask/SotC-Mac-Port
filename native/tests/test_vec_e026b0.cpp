#include <cstdio>
#include <cstdlib>

#include "core/vec_e026b0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecE026b0 g{1, 2, 3, 4};
    init_vec_e026b0(g);
    CHECK(g == kVecE026b0Bits);

    g = {9, 9, 9, 9};
    init_vec_e026b0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecE026b0Bits[0], kVecE026b0Bits[1], kVecE026b0Bits[2], kVecE026b0Bits[3],
                    0x1312f10ull, 0x3353350ull, 0xe026b0ull, 53u);
    } else {
        std::puts("vec_e026b0: all checks passed");
    }
    return 0;
}
