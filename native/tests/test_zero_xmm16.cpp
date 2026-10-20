#include <cstdio>
#include <cstdlib>

#include "core/zero_xmm16.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    sotc::Xmm16 cell{1, 2, 3, 4};
    sotc::zero_xmm16(cell);
    CHECK((cell == sotc::Xmm16{0, 0, 0, 0}));
    cell = {0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu};
    sotc::zero_xmm16(cell);
    CHECK((cell == sotc::Xmm16{0, 0, 0, 0}));
    std::puts("zero_xmm16: all checks passed");
    return 0;
}
