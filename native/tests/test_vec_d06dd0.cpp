#include <cstdio>
#include <cstdlib>

#include "core/vec_d06dd0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD06dd0 g{1, 2, 3, 4};
    init_vec_d06dd0(g);
    CHECK(g == kVecD06dd0Bits);

    g = {9, 9, 9, 9};
    init_vec_d06dd0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD06dd0Bits[0], kVecD06dd0Bits[1], kVecD06dd0Bits[2], kVecD06dd0Bits[3],
                    0x12fc9b0ull, 0x3351dd0ull, 0xd06dd0ull, 135u);
    } else {
        std::puts("vec_d06dd0: all checks passed");
    }
    return 0;
}
