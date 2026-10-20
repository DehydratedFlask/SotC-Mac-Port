#include <cstdio>
#include <cstdlib>

#include "core/vec_c17a70.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC17A70 g{1, 2, 3, 4};
    init_vec_c17a70(g);
    CHECK(g == kVecC17A70Bits);

    g = {9, 9, 9, 9};
    init_vec_c17a70(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC17A70Bits[0], kVecC17A70Bits[1], kVecC17A70Bits[2], kVecC17A70Bits[3],
                    0x12e9230ull, 0x3351530ull, 0xc17a70ull, 224u);
    } else {
        std::puts("vec_c17a70: all checks passed");
    }
    return 0;
}
