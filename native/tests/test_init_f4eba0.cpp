#include <cstdio>
#include <cstdlib>
#include "core/init_f4eba0.hpp"
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)
int main(int argc, char**) {
    using namespace sotc;
    InitF4eba0 g{};
    init_f4eba0(g);
    CHECK(g.ones == ~0ull && g.ones_from_nan == ~0ull && g.zero == 0);
    CHECK(g.q1 == 0xffffffff00000000ull);
    CHECK(g.q2 == 0xff7fffffff7fffffull);
    CHECK(g.f2_qword == 0xff7fffffull);
    for (auto e : g.epsilon_x4) CHECK(e == 0x34000000u);
    if (argc > 1) std::printf("{\"q1\":%llu,\"q2\":%llu,\"f2\":%llu,\"eps0\":%u}\n",(unsigned long long)g.q1,(unsigned long long)g.q2,(unsigned long long)g.f2_qword,g.epsilon_x4[0]);
    else std::puts("init_f4eba0: all checks passed");
    return 0;
}
