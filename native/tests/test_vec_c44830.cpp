#include <cstdio>
#include <cstdlib>

#include "core/vec_c44830.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC44830 g{1, 2, 3, 4};
    init_vec_c44830(g);
    CHECK(g == kVecC44830Bits);

    g = {9, 9, 9, 9};
    init_vec_c44830(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC44830Bits[0], kVecC44830Bits[1], kVecC44830Bits[2], kVecC44830Bits[3],
                    0x12ed4f0ull, 0x3351620ull, 0xc44830ull, 213u);
    } else {
        std::puts("vec_c44830: all checks passed");
    }
    return 0;
}
