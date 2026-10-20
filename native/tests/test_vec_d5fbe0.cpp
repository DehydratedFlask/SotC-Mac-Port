#include <cstdio>
#include <cstdlib>

#include "core/vec_d5fbe0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD5fbe0 g{1, 2, 3, 4};
    init_vec_d5fbe0(g);
    CHECK(g == kVecD5fbe0Bits);

    g = {9, 9, 9, 9};
    init_vec_d5fbe0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD5fbe0Bits[0], kVecD5fbe0Bits[1], kVecD5fbe0Bits[2], kVecD5fbe0Bits[3],
                    0x1303920ull, 0x3352930ull, 0xd5fbe0ull, 104u);
    } else {
        std::puts("vec_d5fbe0: all checks passed");
    }
    return 0;
}
