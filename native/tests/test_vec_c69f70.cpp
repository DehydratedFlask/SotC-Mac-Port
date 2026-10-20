#include <cstdio>
#include <cstdlib>

#include "core/vec_c69f70.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC69f70 g{1, 2, 3, 4};
    init_vec_c69f70(g);
    CHECK(g == kVecC69f70Bits);

    g = {9, 9, 9, 9};
    init_vec_c69f70(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC69f70Bits[0], kVecC69f70Bits[1], kVecC69f70Bits[2], kVecC69f70Bits[3],
                    0x12f0a30ull, 0x3351800ull, 0xc69f70ull, 191u);
    } else {
        std::puts("vec_c69f70: all checks passed");
    }
    return 0;
}
