#include <cstdio>
#include <cstdlib>

#include "core/vec_ef85e0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecEf85e0 g{1, 2, 3, 4};
    init_vec_ef85e0(g);
    CHECK(g == kVecEf85e0Bits);

    g = {9, 9, 9, 9};
    init_vec_ef85e0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecEf85e0Bits[0], kVecEf85e0Bits[1], kVecEf85e0Bits[2], kVecEf85e0Bits[3],
                    0x1324f80ull, 0x3354180ull, 0xef85e0ull, 20u);
    } else {
        std::puts("vec_ef85e0: all checks passed");
    }
    return 0;
}
