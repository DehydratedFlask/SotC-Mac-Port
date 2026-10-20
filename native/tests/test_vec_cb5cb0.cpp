#include <cstdio>
#include <cstdlib>

#include "core/vec_cb5cb0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCb5cb0 g{1, 2, 3, 4};
    init_vec_cb5cb0(g);
    CHECK(g == kVecCb5cb0Bits);

    g = {9, 9, 9, 9};
    init_vec_cb5cb0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCb5cb0Bits[0], kVecCb5cb0Bits[1], kVecCb5cb0Bits[2], kVecCb5cb0Bits[3],
                    0x12f4c80ull, 0x3351bd0ull, 0xcb5cb0ull, 167u);
    } else {
        std::puts("vec_cb5cb0: all checks passed");
    }
    return 0;
}
