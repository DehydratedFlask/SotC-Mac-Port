#include <cstdio>
#include <cstdlib>

#include "core/vec_c6d910.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC6d910 g{1, 2, 3, 4};
    init_vec_c6d910(g);
    CHECK(g == kVecC6d910Bits);

    g = {9, 9, 9, 9};
    init_vec_c6d910(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC6d910Bits[0], kVecC6d910Bits[1], kVecC6d910Bits[2], kVecC6d910Bits[3],
                    0x12f0ea0ull, 0x3351820ull, 0xc6d910ull, 189u);
    } else {
        std::puts("vec_c6d910: all checks passed");
    }
    return 0;
}
