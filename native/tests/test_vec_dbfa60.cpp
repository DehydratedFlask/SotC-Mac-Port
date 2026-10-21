#include <cstdio>
#include <cstdlib>

#include "core/vec_dbfa60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecDBFA60 g{1, 2, 3, 4};
    init_vec_dbfa60(g);
    CHECK(g == kVecDBFA60Bits);

    g = {9, 9, 9, 9};
    init_vec_dbfa60(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecDBFA60Bits[0], kVecDBFA60Bits[1], kVecDBFA60Bits[2], kVecDBFA60Bits[3],
                    0x130de20ull, 0x3352ca0ull, 0xdbfa60ull, 70u);
    } else {
        std::puts("vec_dbfa60: all checks passed");
    }
    return 0;
}
