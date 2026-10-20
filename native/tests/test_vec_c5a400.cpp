#include <cstdio>
#include <cstdlib>

#include "core/vec_c5a400.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC5A400 g{1, 2, 3, 4};
    init_vec_c5a400(g);
    CHECK(g == kVecC5A400Bits);

    g = {9, 9, 9, 9};
    init_vec_c5a400(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC5A400Bits[0], kVecC5A400Bits[1], kVecC5A400Bits[2], kVecC5A400Bits[3],
                    0x12eee30ull, 0x33516c0ull, 0xc5a400ull, 203u);
    } else {
        std::puts("vec_c5a400: all checks passed");
    }
    return 0;
}
