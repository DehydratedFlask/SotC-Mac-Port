#include <cstdio>
#include <cstdlib>

#include "core/vec_de1ad0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDe1ad0 g{1, 2, 3, 4};
    init_vec_de1ad0(g);
    CHECK(g == kVecDe1ad0Bits);

    g = {9, 9, 9, 9};
    init_vec_de1ad0(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDe1ad0Bits[0], kVecDe1ad0Bits[1], kVecDe1ad0Bits[2], kVecDe1ad0Bits[3],
                    0x130fd00ull, 0x3353300ull, 0xde1ad0ull, 58u);
    } else {
        std::puts("vec_de1ad0: all checks passed");
    }
    return 0;
}
