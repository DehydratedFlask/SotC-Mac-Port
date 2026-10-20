#include <cstdio>
#include <cstdlib>

#include "core/vec_c54830.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecC54830 g{1, 2, 3, 4};
    init_vec_c54830(g);
    CHECK(g == kVecC54830Bits);

    g = {9, 9, 9, 9};
    init_vec_c54830(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecC54830Bits[0], kVecC54830Bits[1], kVecC54830Bits[2], kVecC54830Bits[3],
                    0x12ee5f0ull, 0x3351680ull, 0xc54830ull, 207u);
    } else {
        std::puts("vec_c54830: all checks passed");
    }
    return 0;
}
