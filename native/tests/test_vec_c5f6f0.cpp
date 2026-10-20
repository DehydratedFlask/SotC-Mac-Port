#include <cstdio>
#include <cstdlib>

#include "core/vec_c5f6f0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC5f6f0 g{1, 2, 3, 4};
    init_vec_c5f6f0(g);
    CHECK(g == kVecC5f6f0Bits);

    g = {9, 9, 9, 9};
    init_vec_c5f6f0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC5f6f0Bits[0], kVecC5f6f0Bits[1], kVecC5f6f0Bits[2], kVecC5f6f0Bits[3],
                    0x12ef650ull, 0x3351700ull, 0xc5f6f0ull, 199u);
    } else {
        std::puts("vec_c5f6f0: all checks passed");
    }
    return 0;
}
