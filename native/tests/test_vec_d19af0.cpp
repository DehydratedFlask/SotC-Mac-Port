#include <cstdio>
#include <cstdlib>

#include "core/vec_d19af0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD19af0 g{1, 2, 3, 4};
    init_vec_d19af0(g);
    CHECK(g == kVecD19af0Bits);

    g = {9, 9, 9, 9};
    init_vec_d19af0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD19af0Bits[0], kVecD19af0Bits[1], kVecD19af0Bits[2], kVecD19af0Bits[3],
                    0x12fec00ull, 0x3351e70ull, 0xd19af0ull, 67u);
    } else {
        std::puts("vec_d19af0: all checks passed");
    }
    return 0;
}
