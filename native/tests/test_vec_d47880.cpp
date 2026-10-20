#include <cstdio>
#include <cstdlib>

#include "core/vec_d47880.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD47880 g{1, 2, 3, 4};
    init_vec_d47880(g);
    CHECK(g == kVecD47880Bits);

    g = {9, 9, 9, 9};
    init_vec_d47880(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD47880Bits[0], kVecD47880Bits[1], kVecD47880Bits[2], kVecD47880Bits[3],
                    0x1302d80ull, 0x3352110ull, 0xd47880ull, 106u);
    } else {
        std::puts("vec_d47880: all checks passed");
    }
    return 0;
}
