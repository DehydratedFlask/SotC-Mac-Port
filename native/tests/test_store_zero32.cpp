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
    std::uint32_t guarded[] = {0x11223344u, 0xA5A5A5A5u, 0x55667788u};
    sotc::store_zero32(guarded[1]);
    CHECK(guarded[0] == 0x11223344u && guarded[1] == 0u && guarded[2] == 0x55667788u);
    std::puts("store_zero32: all checks passed");
    return 0;
}
