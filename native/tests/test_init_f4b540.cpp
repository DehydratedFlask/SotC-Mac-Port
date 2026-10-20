#include <cstdio>
#include <cstdlib>
#include "core/init_f4b540.hpp"
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)
int main(int argc, char**) {
    using namespace sotc;
    InitF4b540 g{};
    g.fill(0xA5);
    init_f4b540(g);
    CHECK(g.size() == 240);
    CHECK(g[0] == 0x00 && g[3] == 0x80);
    CHECK(g[0x20] == 0x00 && g[0x23] == 0x34);
    CHECK(g[0x50] == 0 && g[0x70] == 0);
    CHECK(g[0x80] == 0x00 && g[0x83] == 0x3f);
    if (argc > 1) std::printf("{\"len\":%zu,\"b0\":%u,\"b32\":%u,\"b128\":%u}\n", g.size(), g[0], g[32], g[128]);
    else std::puts("init_f4b540: all checks passed");
    return 0;
}
