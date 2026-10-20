#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <thread>

#include "core/clear_flag32.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    std::atomic<std::uint32_t> cell{0xA5A5A5A5u};
    sotc::clear_flag32(cell);
    CHECK(cell.load() == 0);
    sotc::clear_flag32(cell);
    CHECK(cell.load() == 0);

    cell.store(0x11223344u);
    std::atomic<bool> go{false}, torn{false};
    std::thread reader([&] {
        while (!go.load()) {}
        for (int i = 0; i < 100000; ++i) {
            auto v = cell.load();
            if (v != 0x11223344u && v != 0) torn = true;
        }
    });
    go = true;
    sotc::clear_flag32(cell);
    reader.join();
    CHECK(!torn.load() && cell.load() == 0);

    std::puts("clear_flag32: all checks passed");
    return 0;
}
