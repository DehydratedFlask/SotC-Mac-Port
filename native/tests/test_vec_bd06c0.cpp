#include <cstdio>
#include <cstdlib>

#include "core/vec_bd06c0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBD06C0 g{1, 2, 3, 4};
    init_vec_bd06c0(g);
    CHECK(g == kVecBD06C0Bits);

    g = {9, 9, 9, 9};
    init_vec_bd06c0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBD06C0Bits[0], kVecBD06C0Bits[1], kVecBD06C0Bits[2], kVecBD06C0Bits[3],
                    0x12e0ee0ull, 0x3351160ull, 0xbd06c0ull, 248u);
    } else {
        std::puts("vec_bd06c0: all checks passed");
    }
    return 0;
}
