#include <cstdio>
#include <cstdlib>
#include "core/vec_cee8a0.hpp"
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)
int main(int argc, char**) {
    using namespace sotc;
    VecCee8a0 g{1,2,3,4};
    init_vec_cee8a0(g);
    CHECK(g == kVecCee8a0Bits);
    g = {9,9,9,9};
    init_vec_cee8a0(g);
    CHECK(g[0]==0xffffffffu && g[1]==0xffffffffu && g[2]==0xffffffffu && g[3]==0);
    if (argc > 1) {
        std::printf("{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"value\":%llu,\"rva\":%llu,\"index\":%u}\n",
            kVecCee8a0Bits[0],kVecCee8a0Bits[1],kVecCee8a0Bits[2],kVecCee8a0Bits[3],
            19898048ull,53812592ull,0xcee8a0ull,137u);
    } else std::puts("vec_cee8a0: all checks passed");
    return 0;
}
