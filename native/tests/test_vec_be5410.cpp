#include <cstdio>
#include <cstdlib>

#include "core/vec_be5410.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBE5410 g{1, 2, 3, 4};
    init_vec_be5410(g);
    CHECK(g == kVecBE5410Bits);

    g = {9, 9, 9, 9};
    init_vec_be5410(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBE5410Bits[0], kVecBE5410Bits[1], kVecBE5410Bits[2], kVecBE5410Bits[3],
                    0x12e35c0ull, 0x3351280ull, 0xbe5410ull, 238u);
    } else {
        std::puts("vec_be5410: all checks passed");
    }
    return 0;
}
