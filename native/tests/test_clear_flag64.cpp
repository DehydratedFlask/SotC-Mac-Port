#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <thread>

#include "core/clear_flag64.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    std::atomic<std::uint64_t> cell{0xA5A5A5A5A5A5A5A5ull};
    sotc::clear_flag64(cell);
    CHECK(cell.load() == 0);
    sotc::clear_flag64(cell);
    CHECK(cell.load() == 0);

    cell.store(0x1122334455667788ull);
    std::atomic<bool> go{false}, torn{false};
    std::thread reader([&] {
        while (!go.load()) {}
        for (int i = 0; i < 100000; ++i) {
            auto v = cell.load();
            if (v != 0x1122334455667788ull && v != 0) torn = true;
        }
    });
    go = true;
    sotc::clear_flag64(cell);
    reader.join();
    CHECK(!torn.load() && cell.load() == 0);

    std::puts("clear_flag64: all checks passed");
    return 0;
}
