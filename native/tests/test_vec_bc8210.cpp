#include <cstdio>
#include <cstdlib>

#include "core/vec_bc8210.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecBC8210 g{1, 2, 3, 4};
    init_vec_bc8210(g);
    CHECK(g == kVecBC8210Bits);

    g = {9, 9, 9, 9};
    init_vec_bc8210(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecBC8210Bits[0], kVecBC8210Bits[1], kVecBC8210Bits[2], kVecBC8210Bits[3],
                    0x12dfc60ull, 0x3351100ull, 0xbc8210ull, 254u);
    } else {
        std::puts("vec_bc8210: all checks passed");
    }
    return 0;
}
