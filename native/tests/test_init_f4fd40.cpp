#include <cstdio>
#include <cstdlib>
#include "core/init_f4fd40.hpp"
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)
int main(int argc, char**) {
    using namespace sotc;
    InitF4fd40 g{};
    init_f4fd40(g);
    CHECK(g.ones0 == ~0ull && g.ones1 == ~0ull);
    for (auto e : g.epsilon_x4) CHECK(e == kEpsilonBits);
    if (argc > 1) std::printf("{\"ones0\":%llu,\"ones1\":%llu,\"eps0\":%u}\n",(unsigned long long)g.ones0,(unsigned long long)g.ones1,g.epsilon_x4[0]);
    else std::puts("init_f4fd40: all checks passed");
    return 0;
}
