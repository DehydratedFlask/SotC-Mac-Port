// The recovered cells are compared with an independent byte oracle, with a poisoned buffer (bytes no
// store covers must stay untouched, and the two atomic cells must be overwritten even when they held
// non-zero data, as an exchange does), and with a two-thread run showing the exchange is atomic
// (a concurrent writer never produces a torn 32-bit value). --dump feeds
// tools/lift/check_f7ce8d0.py (binary-vs-native tie).
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

#include "core/leaf_7ce8d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
constexpr std::size_t kSize = sotc::kLeaf7ce8d0RegionBytes;   // 52
constexpr std::uint8_t kPoison = 0xAA;
// Expected bytes of the written cells (little endian 0x3f80000000000001 = 01 00 00 00 00 00 80 3f).
constexpr std::array<std::uint8_t, 8> kWord = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f};
// 1 where some store of the original covers the byte.
constexpr std::array<std::uint8_t, kSize> kCovered = {
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1,
};

std::array<std::uint8_t, kSize> expected(std::uint8_t fill) {
    std::array<std::uint8_t, kSize> out;
    out.fill(fill);
    for (std::size_t i = 0; i != 8; ++i) { out[0x00 + i] = kWord[i]; out[0x20 + i] = kWord[i]; }
    for (std::size_t i = 0; i != 4; ++i) { out[0x10 + i] = 0; out[0x30 + i] = 0; }
    return out;
}

std::array<std::uint8_t, kSize> bytes_of(const sotc::Leaf7ce8d0& g) {
    std::array<std::uint8_t, kSize> out;
    std::memcpy(out.data(), static_cast<const void*>(&g), kSize);
    return out;
}
void fill(sotc::Leaf7ce8d0& g, std::uint8_t b) { std::memset(static_cast<void*>(&g), b, sizeof g); }
}  // namespace

int main(int argc, char**) {
    using namespace sotc;
    static_assert(offsetof(Leaf7ce8d0, word0) == 0x00 && offsetof(Leaf7ce8d0, flag0) == 0x10 &&
                  offsetof(Leaf7ce8d0, word1) == 0x20 && offsetof(Leaf7ce8d0, flag1) == 0x30);
    static_assert(offsetof(Leaf7ce8d0, flag1) + sizeof(std::uint32_t) == kLeaf7ce8d0RegionBytes);
    static_assert(std::atomic<std::uint32_t>::is_always_lock_free);
    CHECK(kLeaf7ce8d0AtomicOffsets[0] == offsetof(Leaf7ce8d0, flag0) && kLeaf7ce8d0AtomicOffsets[1] == offsetof(Leaf7ce8d0, flag1));

    Leaf7ce8d0 g{};
    init_leaf_7ce8d0(g);
    CHECK(bytes_of(g) == expected(0));
    CHECK(g.word0 == 0x3f80000000000001ull && g.word1 == 0x3f80000000000001ull);
    CHECK(g.flag0.load() == 0 && g.flag1.load() == 0);
    float f = 0;                                    // high dword is 1.0f, low dword is 1
    std::uint32_t hi = static_cast<std::uint32_t>(g.word0 >> 32), lo = static_cast<std::uint32_t>(g.word0);
    std::memcpy(&f, &hi, 4);
    CHECK(f == 1.0f && lo == 1u);

    // Poisoned buffer: covered bytes rewritten (flags overwritten to 0), others untouched.
    Leaf7ce8d0 p;
    fill(p, kPoison);
    init_leaf_7ce8d0(p);
    const auto got = bytes_of(p);
    const auto want = expected(kPoison);
    for (std::size_t i = 0; i != kSize; ++i) {
        CHECK(got[i] == want[i]);
        CHECK(kCovered[i] ? true : got[i] == kPoison);
    }
    init_leaf_7ce8d0(p);   // idempotent
    CHECK(bytes_of(p) == want);

    // Exchange semantics: flags that held non-zero values end at zero; the old value is discarded.
    Leaf7ce8d0 q;
    fill(q, 0xFF);
    init_leaf_7ce8d0(q);
    CHECK(q.flag0.load() == 0 && q.flag1.load() == 0);
    CHECK(q.unwritten0[0] == 0xFF && q.unwritten1[11] == 0xFF && q.unwritten2[7] == 0xFF);

    // Concurrency: with another thread storing non-zero values into the flag, every observation is
    // an intact 32-bit value (0 or the writer's value) and the constructor's last write wins.
    Leaf7ce8d0 r{};
    std::atomic<bool> stop{false};
    std::thread writer([&] { while (!stop.load()) { r.flag0.store(0xdeadbeefu); r.flag1.store(0xdeadbeefu); } });
    for (int i = 0; i != 2000; ++i) {
        init_leaf_7ce8d0(r);
        const auto a = r.flag0.load();
        CHECK(a == 0u || a == 0xdeadbeefu);
    }
    stop.store(true);
    writer.join();
    init_leaf_7ce8d0(r);
    CHECK(r.flag0.load() == 0 && r.flag1.load() == 0);

    if (argc > 1) {
        std::printf("{\"rva\":%u,\"body_length\":%u,\"base\":%llu,\"bytes\":\"", 0x7ce8d0u, 41u,
                    static_cast<unsigned long long>(0x325e470ull));
        for (auto byte : bytes_of(g)) std::printf("%02x", byte);
        std::printf("\",\"bytes_poisoned\":\"");
        for (auto byte : bytes_of(p)) std::printf("%02x", byte);
        std::printf("\",\"atomic_offsets\":[%zu,%zu]}\n", kLeaf7ce8d0AtomicOffsets[0], kLeaf7ce8d0AtomicOffsets[1]);
    } else {
        std::puts("leaf_7ce8d0: all checks passed");
    }
    return 0;
}
