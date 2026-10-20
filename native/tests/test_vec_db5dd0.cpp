#include <cstdio>
#include <cstdlib>

#include "core/vec_db5dd0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDb5dd0 g{1, 2, 3, 4};
    init_vec_db5dd0(g);
    CHECK(g == kVecDb5dd0Bits);

    g = {9, 9, 9, 9};
    init_vec_db5dd0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDb5dd0Bits[0], kVecDb5dd0Bits[1], kVecDb5dd0Bits[2], kVecDb5dd0Bits[3],
                    0x130d380ull, 0x3352c60ull, 0xdb5dd0ull, 74u);
    } else {
        std::puts("vec_db5dd0: all checks passed");
    }
    return 0;
}
