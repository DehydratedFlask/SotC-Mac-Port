#include <cstdio>
#include <cstdlib>

#include "core/vec_db7e60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDb7e60 g{1, 2, 3, 4};
    init_vec_db7e60(g);
    CHECK(g == kVecDb7e60Bits);

    g = {9, 9, 9, 9};
    init_vec_db7e60(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDb7e60Bits[0], kVecDb7e60Bits[1], kVecDb7e60Bits[2], kVecDb7e60Bits[3],
                    0x130d730ull, 0x3352c70ull, 0xdb7e60ull, 73u);
    } else {
        std::puts("vec_db7e60: all checks passed");
    }
    return 0;
}
