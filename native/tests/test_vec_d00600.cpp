#include <cstdio>
#include <cstdlib>

#include "core/vec_d00600.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD00600 g{1, 2, 3, 4};
    init_vec_d00600(g);
    CHECK(g == kVecD00600Bits);

    g = {9, 9, 9, 9};
    init_vec_d00600(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD00600Bits[0], kVecD00600Bits[1], kVecD00600Bits[2], kVecD00600Bits[3],
                    0x12fb8a0ull, 0x3351db0ull, 0xd00600ull, 137u);
    } else {
        std::puts("vec_d00600: all checks passed");
    }
    return 0;
}
