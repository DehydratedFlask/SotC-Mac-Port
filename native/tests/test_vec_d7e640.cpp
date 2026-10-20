#include <cstdio>
#include <cstdlib>

#include "core/vec_d7e640.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    VecD7e640 g{1, 2, 3, 4};
    init_vec_d7e640(g);
    CHECK(g == kVecD7e640Bits);

    g = {9, 9, 9, 9};
    init_vec_d7e640(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu && g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
                    kVecD7e640Bits[0], kVecD7e640Bits[1], kVecD7e640Bits[2], kVecD7e640Bits[3],
                    0x1307200ull, 0x3352a30ull, 0xd7e640ull, 88u);
    } else {
        std::puts("vec_d7e640: all checks passed");
    }
    return 0;
}
