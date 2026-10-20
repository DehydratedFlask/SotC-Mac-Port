#include <cstdio>
#include <cstdlib>

#include "core/vec_c42b00.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC42B00 g{1, 2, 3, 4};
    init_vec_c42b00(g);
    CHECK(g == kVecC42B00Bits);

    g = {9, 9, 9, 9};
    init_vec_c42b00(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC42B00Bits[0], kVecC42B00Bits[1], kVecC42B00Bits[2], kVecC42B00Bits[3],
                    0x12ed210ull, 0x3351610ull, 0xc42b00ull, 214u);
    } else {
        std::puts("vec_c42b00: all checks passed");
    }
    return 0;
}
