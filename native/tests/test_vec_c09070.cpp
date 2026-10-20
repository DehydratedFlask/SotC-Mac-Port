#include <cstdio>
#include <cstdlib>

#include "core/vec_c09070.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC09070 g{1, 2, 3, 4};
    init_vec_c09070(g);
    CHECK(g == kVecC09070Bits);

    g = {9, 9, 9, 9};
    init_vec_c09070(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC09070Bits[0], kVecC09070Bits[1], kVecC09070Bits[2], kVecC09070Bits[3],
                    0x12e6f40ull, 0x33512c0ull, 0xc09070ull, 235u);
    } else {
        std::puts("vec_c09070: all checks passed");
    }
    return 0;
}
