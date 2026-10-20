#include <cstdio>
#include <cstdlib>

#include "core/vec_c4e5e0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC4E5E0 g{1, 2, 3, 4};
    init_vec_c4e5e0(g);
    CHECK(g == kVecC4E5E0Bits);

    g = {9, 9, 9, 9};
    init_vec_c4e5e0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC4E5E0Bits[0], kVecC4E5E0Bits[1], kVecC4E5E0Bits[2], kVecC4E5E0Bits[3],
                    0x12edf70ull, 0x3351660ull, 0xc4e5e0ull, 209u);
    } else {
        std::puts("vec_c4e5e0: all checks passed");
    }
    return 0;
}
