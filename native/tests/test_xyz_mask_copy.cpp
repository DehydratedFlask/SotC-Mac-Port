#include <cstdio>
#include <cstdlib>

#include "core/xyz_mask_copy.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    XyzMaskCopy g{1, 2, 3, 4};
    init_xyz_mask_copy(g);
    CHECK(g == kXyzMaskBits);

    // Unguarded: always rewrites.
    g = {9, 9, 9, 9};
    init_xyz_mask_copy(g);
    CHECK(g == kXyzMaskBits);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u]}\n", g[0], g[1], g[2], g[3]);
    } else {
        std::puts("xyz_mask_copy: all checks passed");
    }
    return 0;
}
