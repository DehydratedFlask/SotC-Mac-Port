#include <cstdio>
#include <cstdlib>

#include "core/vec_dac410.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDac410 g{1, 2, 3, 4};
    init_vec_dac410(g);
    CHECK(g == kVecDac410Bits);

    g = {9, 9, 9, 9};
    init_vec_dac410(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDac410Bits[0], kVecDac410Bits[1], kVecDac410Bits[2], kVecDac410Bits[3],
                    0x130ca00ull, 0x3352c30ull, 0xdac410ull, 77u);
    } else {
        std::puts("vec_dac410: all checks passed");
    }
    return 0;
}
