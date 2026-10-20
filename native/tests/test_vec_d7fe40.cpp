#include <cstdio>
#include <cstdlib>

#include "core/vec_d7fe40.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD7fe40 g{1, 2, 3, 4};
    init_vec_d7fe40(g);
    CHECK(g == kVecD7fe40Bits);

    g = {9, 9, 9, 9};
    init_vec_d7fe40(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD7fe40Bits[0], kVecD7fe40Bits[1], kVecD7fe40Bits[2], kVecD7fe40Bits[3],
                    0x13077e0ull, 0x3352a40ull, 0xd7fe40ull, 87u);
    } else {
        std::puts("vec_d7fe40: all checks passed");
    }
    return 0;
}
