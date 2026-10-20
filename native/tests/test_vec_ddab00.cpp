#include <cstdio>
#include <cstdlib>

#include "core/vec_ddab00.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDdab00 g{1, 2, 3, 4};
    init_vec_ddab00(g);
    CHECK(g == kVecDdab00Bits);

    g = {9, 9, 9, 9};
    init_vec_ddab00(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDdab00Bits[0], kVecDdab00Bits[1], kVecDdab00Bits[2], kVecDdab00Bits[3],
                    0x130f090ull, 0x3352eb0ull, 0xddab00ull, 62u);
    } else {
        std::puts("vec_ddab00: all checks passed");
    }
    return 0;
}
