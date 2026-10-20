#include <cstdio>
#include <cstdlib>

#include "core/vec_c23650.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC23650 g{1, 2, 3, 4};
    init_vec_c23650(g);
    CHECK(g == kVecC23650Bits);

    g = {9, 9, 9, 9};
    init_vec_c23650(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC23650Bits[0], kVecC23650Bits[1], kVecC23650Bits[2], kVecC23650Bits[3],
                    0x12eaae0ull, 0x3351580ull, 0xc23650ull, 219u);
    } else {
        std::puts("vec_c23650: all checks passed");
    }
    return 0;
}
