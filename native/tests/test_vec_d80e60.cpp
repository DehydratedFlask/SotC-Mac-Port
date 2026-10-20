#include <cstdio>
#include <cstdlib>

#include "core/vec_d80e60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD80e60 g{1, 2, 3, 4};
    init_vec_d80e60(g);
    CHECK(g == kVecD80e60Bits);

    g = {9, 9, 9, 9};
    init_vec_d80e60(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD80e60Bits[0], kVecD80e60Bits[1], kVecD80e60Bits[2], kVecD80e60Bits[3],
                    0x1307f70ull, 0x3352a60ull, 0xd80e60ull, 85u);
    } else {
        std::puts("vec_d80e60: all checks passed");
    }
    return 0;
}
