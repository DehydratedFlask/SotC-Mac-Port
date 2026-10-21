#include <cstdio>
#include <cstdlib>

#include "core/vec_d85490.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD85490 g{1, 2, 3, 4};
    init_vec_d85490(g);
    CHECK(g == kVecD85490Bits);

    g = {9, 9, 9, 9};
    init_vec_d85490(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD85490Bits[0], kVecD85490Bits[1], kVecD85490Bits[2], kVecD85490Bits[3],
                    0x130a970ull, 0x3352a90ull, 0xd85490ull, 82u);
    } else {
        std::puts("vec_d85490: all checks passed");
    }
    return 0;
}
