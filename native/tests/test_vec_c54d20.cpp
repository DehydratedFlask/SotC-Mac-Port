#include <cstdio>
#include <cstdlib>

#include "core/vec_c54d20.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC54D20 g{1, 2, 3, 4};
    init_vec_c54d20(g);
    CHECK(g == kVecC54D20Bits);

    g = {9, 9, 9, 9};
    init_vec_c54d20(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC54D20Bits[0], kVecC54D20Bits[1], kVecC54D20Bits[2], kVecC54D20Bits[3],
                    0x12ee790ull, 0x3351690ull, 0xc54d20ull, 206u);
    } else {
        std::puts("vec_c54d20: all checks passed");
    }
    return 0;
}
