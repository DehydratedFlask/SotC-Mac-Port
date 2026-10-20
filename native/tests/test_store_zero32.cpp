#include <cstdio>
#include <cstdlib>

#include "core/store_zero32.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    std::uint32_t cell = 0xA5A5A5A5u;
    sotc::store_zero32(cell);
    CHECK(cell == 0);
    cell = 1;
    sotc::store_zero32(cell);
    CHECK(cell == 0);
    std::puts("store_zero32: all checks passed");
    return 0;
}
