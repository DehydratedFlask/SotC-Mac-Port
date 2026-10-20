#include <cstdio>
#include <cstdlib>

#include "core/vec_c91b10.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC91b10 g{1, 2, 3, 4};
    init_vec_c91b10(g);
    CHECK(g == kVecC91b10Bits);

    g = {9, 9, 9, 9};
    init_vec_c91b10(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC91b10Bits[0], kVecC91b10Bits[1], kVecC91b10Bits[2], kVecC91b10Bits[3],
                    0x12f2fd0ull, 0x3351a30ull, 0xc91b10ull, 177u);
    } else {
        std::puts("vec_c91b10: all checks passed");
    }
    return 0;
}
