#include <cstdio>
#include <cstdlib>

#include "core/vec_e085e0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecE085e0 g{1, 2, 3, 4};
    init_vec_e085e0(g);
    CHECK(g == kVecE085e0Bits);

    g = {9, 9, 9, 9};
    init_vec_e085e0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecE085e0Bits[0], kVecE085e0Bits[1], kVecE085e0Bits[2], kVecE085e0Bits[3],
                    0x1313740ull, 0x3353370ull, 0xe085e0ull, 51u);
    } else {
        std::puts("vec_e085e0: all checks passed");
    }
    return 0;
}
