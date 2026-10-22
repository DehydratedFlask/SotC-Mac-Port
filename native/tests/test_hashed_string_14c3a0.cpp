// 0x14c3a0 copy_hashed, 0x14c4a0 make_hashed, 0xf730 static empty string (native/core/hashed_string_14c3a0).
// Expected values are literals / independent arithmetic. Poisoned destinations prove exactly which bytes
// are written (the 4 gap bytes at +4 and nothing outside the object). --dump feeds check_f14c3a0.py,
// check_f14c4a0.py and check_ff730.py.
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#include "core/hashed_string_14c3a0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
using sotc::HashedStr;
using sotc::SharedStr;

struct alignas(16) Frame {                 // destination with poisoned neighbours on both sides
    std::array<std::uint8_t, 16> before;
    HashedStr obj;
    std::array<std::uint8_t, 16> after;
};
void fill(Frame& f, std::uint8_t p) {
    f.before.fill(p);
    f.after.fill(p);
    std::memset(&f.obj, p, sizeof f.obj);
}
bool all(const std::uint8_t* b, std::size_t n, std::uint8_t v) {
    for (std::size_t i = 0; i != n; ++i) if (b[i] != v) return false;
    return true;
}
std::string hex(const void* d, std::size_t n) {
    static const char* h = "0123456789abcdef";
    std::string s;
    for (std::size_t i = 0; i != n; ++i) {
        const auto b = static_cast<const std::uint8_t*>(d)[i];
        s.push_back(h[b >> 4]);
        s.push_back(h[b & 15]);
    }
    return s;
}

alignas(8) std::int64_t g_counter;
const std::uint8_t kText[] = {'a', 'b', 'c', 0x80, 0xff};

void copy_checks() {
    for (std::uint8_t poison : {std::uint8_t{0x00}, std::uint8_t{0xaa}, std::uint8_t{0xff}}) {
        // null ctl: no atomic, all fields copied, gap bytes at +4 untouched
        HashedStr src{};
        std::memset(&src, 0x5c, sizeof src);
        src.hash = 0x01020304u;
        src.str = SharedStr{kText, 5, 0x1122334455667788ll, nullptr};
        Frame f;
        fill(f, poison);
        sotc::copy_hashed_14c3a0(f.obj, src);
        CHECK(f.obj.hash == 0x01020304u);
        CHECK(f.obj.str.data == kText && f.obj.str.size == 5 && f.obj.str.aux == 0x1122334455667788ll && f.obj.str.ctl == nullptr);
        CHECK(all(f.obj.gap04, 4, poison));                        // padding NOT copied
        CHECK(all(f.before.data(), 16, poison) && all(f.after.data(), 16, poison));

        // non-null ctl: counter incremented exactly once per copy
        g_counter = 41;
        src.str.ctl = &g_counter;
        fill(f, poison);
        sotc::copy_hashed_14c3a0(f.obj, src);
        CHECK(g_counter == 42 && f.obj.str.ctl == &g_counter);
        sotc::copy_hashed_14c3a0(f.obj, src);
        CHECK(g_counter == 43);
    }
    // wrap of the counter like `lock inc qword`
    g_counter = INT64_MAX;
    HashedStr src{};
    src.str.ctl = &g_counter;
    HashedStr dst{};
    sotc::copy_hashed_14c3a0(dst, src);
    CHECK(g_counter == INT64_MIN);
    // the alias case dst == src behaves as a plain self-copy plus one increment
    g_counter = 0;
    src.hash = 9;
    sotc::copy_hashed_14c3a0(src, src);
    CHECK(src.hash == 9 && g_counter == 1);
}

void make_checks() {
    // Hash of the span after the copy: FNV-1a("a"..) literal vectors.
    const std::uint8_t foobar[] = {'f', 'o', 'o', 'b', 'a', 'r'};
    for (std::uint8_t poison : {std::uint8_t{0x00}, std::uint8_t{0xaa}}) {
        g_counter = 7;
        const SharedStr src{foobar, 6, -5, &g_counter};
        Frame f;
        fill(f, poison);
        sotc::make_hashed_14c4a0(f.obj, src);
        CHECK(f.obj.hash == 0xbf9cf968u);                          // FNV-1a("foobar")
        CHECK(f.obj.str.data == foobar && f.obj.str.size == 6 && f.obj.str.aux == -5 && f.obj.str.ctl == &g_counter);
        CHECK(g_counter == 8);
        CHECK(all(f.obj.gap04, 4, poison));
        CHECK(all(f.before.data(), 16, poison) && all(f.after.data(), 16, poison));
    }
    // len <= 0: hash is the offset basis, ctl null: no atomic
    for (std::int64_t len : {std::int64_t{0}, std::int64_t{-3}}) {
        Frame f;
        fill(f, 0xaa);
        sotc::make_hashed_14c4a0(f.obj, SharedStr{foobar, len, 0, nullptr});
        CHECK(f.obj.hash == 0x811c9dc5u && f.obj.str.size == len);
    }
    // The hash is computed from the COPIED span (same pointer/length here) and only `len` bytes are used.
    Frame f;
    fill(f, 0);
    sotc::make_hashed_14c4a0(f.obj, SharedStr{foobar, 1, 0, nullptr});
    CHECK(f.obj.hash == (0x811c9dc5u ^ 'f') * 0x01000193u);
}

void empty_checks() {
    const SharedStr& e = sotc::empty_string_f730();
    CHECK(&sotc::empty_string_f730() == &e);                       // singleton
    CHECK(e.size == 0 && e.aux == 0 && e.ctl == nullptr);
    CHECK(e.data == reinterpret_cast<const std::uint8_t*>(&e) + 0x28);   // 0x2dd9f78 - 0x2dd9f50
    CHECK(*e.data == 0);
    constexpr int kThreads = 8;
    std::atomic<int> go{0};
    std::vector<const SharedStr*> seen(kThreads, nullptr);
    std::vector<std::thread> ts;
    for (int t = 0; t != kThreads; ++t) ts.emplace_back([&, t] { while (!go.load()) {} seen[t] = &sotc::empty_string_f730(); });
    go.store(1);
    for (auto& t : ts) t.join();
    for (auto* p : seen) CHECK(p == &e);
}

// --dump: JSON for the checkers. Memory layout as plain hex of the destination objects.
void dump() {
    static const std::uint8_t text[] = {'f', 'o', 'o', 'b', 'a', 'r'};
    HashedStr src{};
    std::memset(&src, 0x5c, sizeof src);
    src.hash = 0xA1B2C3D4u;
    src.str = SharedStr{text, 6, 0x1122334455667788ll, nullptr};
    HashedStr dst{};
    std::memset(&dst, 0xaa, sizeof dst);
    sotc::copy_hashed_14c3a0(dst, src);
    // pointer slots are reported separately: the data pointer is native memory, the checker maps it.
    std::printf("{\"copy\":{\"hash\":%u,\"gap\":\"%s\",\"size\":%lld,\"aux\":%lld,\"data_is_src\":%s,\"ctl_null\":%s},",
                dst.hash, hex(dst.gap04, 4).c_str(), static_cast<long long>(dst.str.size), static_cast<long long>(dst.str.aux),
                dst.str.data == text ? "true" : "false", dst.str.ctl == nullptr ? "true" : "false");
    g_counter = 100;
    src.str.ctl = &g_counter;
    std::memset(&dst, 0xaa, sizeof dst);
    sotc::copy_hashed_14c3a0(dst, src);
    std::printf("\"copy_ctl\":{\"counter\":%lld,\"ctl_is_src\":%s},", static_cast<long long>(g_counter), dst.str.ctl == &g_counter ? "true" : "false");
    // make_hashed on several spans
    std::printf("\"make\":[");
    const std::int64_t lens[] = {6, 1, 0, -1};
    bool first = true;
    for (std::int64_t len : lens) {
        g_counter = 5;
        HashedStr d{};
        std::memset(&d, 0xaa, sizeof d);
        sotc::make_hashed_14c4a0(d, SharedStr{text, len, 0x0102030405060708ll, &g_counter});
        std::printf("%s{\"len\":%lld,\"hash\":%u,\"gap\":\"%s\",\"aux\":%lld,\"counter\":%lld}", first ? "" : ",",
                    static_cast<long long>(len), d.hash, hex(d.gap04, 4).c_str(), static_cast<long long>(d.str.aux),
                    static_cast<long long>(g_counter));
        first = false;
    }
    const SharedStr& e = sotc::empty_string_f730();
    std::printf("],\"empty\":{\"data_offset\":%lld,\"size\":%lld,\"aux\":%lld,\"ctl_null\":%s,\"nul_byte\":%u,\"same_address\":%s}}\n",
                static_cast<long long>(e.data - reinterpret_cast<const std::uint8_t*>(&e)), static_cast<long long>(e.size),
                static_cast<long long>(e.aux), e.ctl == nullptr ? "true" : "false", *e.data,
                &sotc::empty_string_f730() == &e ? "true" : "false");
}
}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--dump") == 0) { dump(); return 0; }
    copy_checks();
    make_checks();
    empty_checks();
    std::puts("hashed_string_14c3a0: all checks passed");
    return 0;
}
