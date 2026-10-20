#include <cstdio>
#include <cstdlib>

#include "core/init_ea52f0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;
    InitEa52f0 g{};
    g.fill(0xA5);
    init_ea52f0(g);
    CHECK(g.size() == 184);
    CHECK(g[0] == 0xff && g[12] == 0x00);           // leading {~0,~0,~0,0}
    CHECK(g[16] == 0x0c && g[24] == 0x10);          // u64 ramp start
    CHECK(g[176] == 0x5c && g[183] == 0x00);        // final qword 0x5c
    if (argc > 1) {
        std::printf("{\"len\":%zu,\"b0\":%u,\"b16\":%u,\"b176\":%u}\n",
                    g.size(), g[0], g[16], g[176]);
    } else {
        std::puts("init_ea52f0: all checks passed");
    }
    return 0;
}
