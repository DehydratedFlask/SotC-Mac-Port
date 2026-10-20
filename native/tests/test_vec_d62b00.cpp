#include <cstdio>
#include <cstdlib>

#include "core/vec_d62b00.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD62b00 g{1, 2, 3, 4};
    init_vec_d62b00(g);
    CHECK(g == kVecD62b00Bits);

    g = {9, 9, 9, 9};
    init_vec_d62b00(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD62b00Bits[0], kVecD62b00Bits[1], kVecD62b00Bits[2], kVecD62b00Bits[3],
                    0x1304610ull, 0x3352960ull, 0xd62b00ull, 101u);
    } else {
        std::puts("vec_d62b00: all checks passed");
    }
    return 0;
}
