#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <new>
#include <string>
#include <thread>
#include <vector>

#include "core/atomic_xadd_b30cf0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

constexpr std::int32_t kMax = std::numeric_limits<std::int32_t>::max();
constexpr std::int32_t kMin = std::numeric_limits<std::int32_t>::min();

void dump() {
    const std::int32_t cells[] = {0, 1, -1, 2, kMax, kMin, kMin + 1, 0x12345678, -0x12345678};
    const std::int32_t deltas[] = {0, 1, -1, 5, -7, kMax, kMin, 0x7fff0000};
    std::printf("{\"dec\":{\"rva\":%u,\"body_length\":12,\"cases\":[", 0xb30cf0u);
    bool first = true;
    for (std::int32_t v : cells) {
        std::atomic<std::int32_t> c{v};
        std::int32_t r = sotc::atomic_dec_fetch_b30cf0(c);
        std::printf("%s{\"init\":%d,\"ret\":%d,\"mem\":%d}", first ? "" : ",", v, r, c.load());
        first = false;
    }
    std::printf("]},\"add\":{\"rva\":%u,\"body_length\":10,\"cases\":[", 0xb30d00u);
    first = true;
    for (std::int32_t v : cells)
        for (std::int32_t d : deltas) {
            std::atomic<std::int32_t> c{v};
            std::int32_t r = sotc::atomic_add_fetch_b30d00(c, d);
            std::printf("%s{\"init\":%d,\"delta\":%d,\"ret\":%d,\"mem\":%d}", first ? "" : ",", v, d, r, c.load());
            first = false;
        }
    std::printf("]}}\n");
}

}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--dump") { dump(); return 0; }

    std::atomic<std::int32_t> c{3};
    CHECK(sotc::atomic_dec_fetch_b30cf0(c) == 2 && c.load() == 2);
    CHECK(sotc::atomic_dec_fetch_b30cf0(c) == 1 && c.load() == 1);
    CHECK(sotc::atomic_dec_fetch_b30cf0(c) == 0 && c.load() == 0);
    CHECK(sotc::atomic_dec_fetch_b30cf0(c) == -1 && c.load() == -1);
    c.store(kMin);
    CHECK(sotc::atomic_dec_fetch_b30cf0(c) == kMax && c.load() == kMax);   // wraps

    c.store(10);
    CHECK(sotc::atomic_add_fetch_b30d00(c, 5) == 15 && c.load() == 15);
    CHECK(sotc::atomic_add_fetch_b30d00(c, -20) == -5 && c.load() == -5);
    CHECK(sotc::atomic_add_fetch_b30d00(c, 0) == -5 && c.load() == -5);
    c.store(kMax);
    CHECK(sotc::atomic_add_fetch_b30d00(c, 1) == kMin && c.load() == kMin);
    CHECK(sotc::atomic_add_fetch_b30d00(c, -1) == kMax && c.load() == kMax);

    // Neighbouring cells are untouched (cells share one aligned block).
    alignas(16) unsigned char raw[16] = {};
    auto* mid = new (raw + 4) std::atomic<std::int32_t>(100);
    new (raw) std::atomic<std::int32_t>(7);
    new (raw + 8) std::atomic<std::int32_t>(9);
    CHECK(sotc::atomic_add_fetch_b30d00(*mid, 11) == 111);
    CHECK(sotc::atomic_dec_fetch_b30cf0(*mid) == 110);
    CHECK(reinterpret_cast<std::atomic<std::int32_t>*>(raw)->load() == 7);
    CHECK(reinterpret_cast<std::atomic<std::int32_t>*>(raw + 8)->load() == 9);

    // Two threads: each does 100000 decrements and 100000 adds of +1; no lost update, final == start.
    {
        std::atomic<std::int32_t> shared{1000};
        std::atomic<bool> go{false};
        auto work = [&] {
            while (!go.load()) {}
            for (int i = 0; i < 100000; ++i) { sotc::atomic_dec_fetch_b30cf0(shared); sotc::atomic_add_fetch_b30d00(shared, 1); }
        };
        std::thread t1(work), t2(work);
        go = true;
        t1.join(); t2.join();
        CHECK(shared.load() == 1000);
    }
    // Returned values are the unique new values: 2 threads x 50000 decrements return 100000 distinct values.
    {
        constexpr int kN = 50000;
        std::atomic<std::int32_t> shared{0};
        std::vector<std::int32_t> r1(kN), r2(kN);
        std::atomic<bool> go{false};
        std::thread t1([&] { while (!go.load()) {} for (int i = 0; i < kN; ++i) r1[i] = sotc::atomic_dec_fetch_b30cf0(shared); });
        std::thread t2([&] { while (!go.load()) {} for (int i = 0; i < kN; ++i) r2[i] = sotc::atomic_dec_fetch_b30cf0(shared); });
        go = true;
        t1.join(); t2.join();
        std::vector<char> seen(2 * kN + 1, 0);
        for (auto v : r1) { CHECK(v <= -1 && v >= -2 * kN); CHECK(!seen[-v]); seen[-v] = 1; }
        for (auto v : r2) { CHECK(v <= -1 && v >= -2 * kN); CHECK(!seen[-v]); seen[-v] = 1; }
        CHECK(shared.load() == -2 * kN);
    }

    std::puts("atomic_xadd_b30cf0: all checks passed");
    return 0;
}
