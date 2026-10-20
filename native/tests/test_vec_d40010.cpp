#include <cstdio>
#include <cstdlib>

#include "core/vec_d40010.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD40010 g{1, 2, 3, 4};
    init_vec_d40010(g);
    CHECK(g == kVecD40010Bits);

    g = {9, 9, 9, 9};
    init_vec_d40010(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD40010Bits[0], kVecD40010Bits[1], kVecD40010Bits[2], kVecD40010Bits[3],
                    0x1301e30ull, 0x33520b0ull, 0xd40010ull, 112u);
    } else {
        std::puts("vec_d40010: all checks passed");
    }
    return 0;
}
