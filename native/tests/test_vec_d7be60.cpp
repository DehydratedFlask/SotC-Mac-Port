#include <cstdio>
#include <cstdlib>

#include "core/vec_d7be60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD7be60 g{1, 2, 3, 4};
    init_vec_d7be60(g);
    CHECK(g == kVecD7be60Bits);

    g = {9, 9, 9, 9};
    init_vec_d7be60(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD7be60Bits[0], kVecD7be60Bits[1], kVecD7be60Bits[2], kVecD7be60Bits[3],
                    0x1306a00ull, 0x33529f0ull, 0xd7be60ull, 92u);
    } else {
        std::puts("vec_d7be60: all checks passed");
    }
    return 0;
}
