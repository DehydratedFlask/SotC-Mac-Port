#include <cstdio>
#include <cstdlib>

#include "core/zero_xmm_qword.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    sotc::Xmm16 xmm{1, 2, 3, 4};
    std::uint64_t q = 0xA5A5A5A5A5A5A5A5ull;
    sotc::zero_xmm_qword(xmm, q);
    CHECK((xmm == sotc::Xmm16{0, 0, 0, 0}));
    CHECK(q == 0);
    xmm = {9, 9, 9, 9};
    q = 1;
    sotc::zero_xmm_qword(xmm, q);
    CHECK((xmm == sotc::Xmm16{0, 0, 0, 0}) && q == 0);
    std::puts("zero_xmm_qword: all checks passed");
    return 0;
}
