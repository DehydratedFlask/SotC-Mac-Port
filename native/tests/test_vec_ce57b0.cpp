#include <cstdio>
#include <cstdlib>

#include "core/vec_ce57b0.hpp"

#define CHECK(c) \
    do { \
        if (!(c)) { \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); \
            std::exit(1); \
        } \
    } while (0)

int main(int argc, char**) {
    using namespace sotc;

    VecCe57B0 cell{1, 2, 3, 4};
    init_vec_ce57b0(cell);
    CHECK(cell == kVecCe57B0Bits);

    // The original constructor has no guard, so a second call overwrites prior contents.
    cell = {9, 8, 7, 6};
    init_vec_ce57b0(cell);
    CHECK(cell == kVecCe57B0Bits);

    if (argc > 1) {
        std::printf(
            "{\"bits\":[%u,%u,%u,%u],\"source\":%u,\"value\":%u,\"rva\":%u,\"index\":%u}\n",
            static_cast<unsigned>(cell[0]), static_cast<unsigned>(cell[1]),
            static_cast<unsigned>(cell[2]), static_cast<unsigned>(cell[3]),
            0x12f8330u, 0x3351cb0u, 0xce57b0u, 153u);
    } else {
        std::puts("vec_ce57b0: all checks passed");
    }
    return 0;
}
