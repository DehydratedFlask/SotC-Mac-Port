#include <cstdio>
#include <cstdlib>

#include "core/vec_cfcc60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecCfcc60 g{1, 2, 3, 4};
    init_vec_cfcc60(g);
    CHECK(g == kVecCfcc60Bits);

    g = {9, 9, 9, 9};
    init_vec_cfcc60(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecCfcc60Bits[0], kVecCfcc60Bits[1], kVecCfcc60Bits[2], kVecCfcc60Bits[3],
                    0x12fb6b0ull, 0x3351da0ull, 0xcfcc60ull, 138u);
    } else {
        std::puts("vec_cfcc60: all checks passed");
    }
    return 0;
}
