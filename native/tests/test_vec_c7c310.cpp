#include <cstdio>
#include <cstdlib>

#include "core/vec_c7c310.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC7c310 g{1, 2, 3, 4};
    init_vec_c7c310(g);
    CHECK(g == kVecC7c310Bits);

    g = {9, 9, 9, 9};
    init_vec_c7c310(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC7c310Bits[0], kVecC7c310Bits[1], kVecC7c310Bits[2], kVecC7c310Bits[3],
                    0x12f2250ull, 0x33519e0ull, 0xc7c310ull, 181u);
    } else {
        std::puts("vec_c7c310: all checks passed");
    }
    return 0;
}
