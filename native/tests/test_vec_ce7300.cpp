#include <cstdio>
#include <cstdlib>

#include "core/vec_ce7300.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;

    VecCe7300 g{1, 2, 3, 4};
    init_vec_ce7300(g);
    CHECK(g == kVecCe7300Bits);

    g = {9, 9, 9, 9};
    init_vec_ce7300(g);

    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu &&
          g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf(
            "{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"destination\":%llu,"
            "\"rva\":%llu,\"survey_index\":%u,\"copy_manifest_index\":%u}\n",
            kVecCe7300Bits[0], kVecCe7300Bits[1], kVecCe7300Bits[2], kVecCe7300Bits[3],
            0x12f8bb0ull, 0x3351cf0ull, 0xce7300ull, 149u, 91u);
    } else {
        std::puts("vec_ce7300: all checks passed");
    }

    return 0;
}
