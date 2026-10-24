#include <cstdio>
#include <cstdlib>

#include "core/vec_d71f60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD71f60 g{1, 2, 3, 4};
    init_vec_d71f60(g);
    CHECK(g == kVecD71f60Bits);

    g = {9, 9, 9, 9};
    init_vec_d71f60(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD71f60Bits[0], kVecD71f60Bits[1], kVecD71f60Bits[2], kVecD71f60Bits[3],
                    0x1305650ull, 0x33529c0ull, 0xd71f60ull, 95u);
    } else {
        std::puts("vec_d71f60: all checks passed");
    }
    return 0;
}
