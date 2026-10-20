#include <cstdio>
#include <cstdlib>

#include "core/zero_struct12.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    sotc::Bytes12 cell{};
    cell.fill(0xA5);
    sotc::zero_struct12(cell);
    for (auto b : cell) CHECK(b == 0);
    cell[0] = 1;
    cell[11] = 2;
    sotc::zero_struct12(cell);
    for (auto b : cell) CHECK(b == 0);
    std::puts("zero_struct12: all checks passed");
    return 0;
}
