#include <cstdio>
#include <cstdlib>

#include "core/vec_b77910.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecB77910 g{1, 2, 3, 4};
    init_vec_b77910(g);
    CHECK(g == kVecB77910Bits);

    g = {9, 9, 9, 9};
    init_vec_b77910(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecB77910Bits[0], kVecB77910Bits[1], kVecB77910Bits[2], kVecB77910Bits[3],
                    0x12d9790ull, 0x3350df0ull, 0xb77910ull, 278u);
    } else {
        std::puts("vec_b77910: all checks passed");
    }
    return 0;
}
