#include <cstdio>
#include <cstdlib>

#include "core/vec_c65f80.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC65f80 g{1, 2, 3, 4};
    init_vec_c65f80(g);
    CHECK(g == kVecC65f80Bits);

    g = {9, 9, 9, 9};
    init_vec_c65f80(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC65f80Bits[0], kVecC65f80Bits[1], kVecC65f80Bits[2], kVecC65f80Bits[3],
                    0x12effa0ull, 0x3351740ull, 0xc65f80ull, 195u);
    } else {
        std::puts("vec_c65f80: all checks passed");
    }
    return 0;
}
