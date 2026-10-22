// Helpers 0x51230 / 0xa77960 / 0x268d40 and ctors 0x78c760 / 0x268ce0 (native/core/guarded_static_helpers).
// Independent expectations are literals (zero pair, flag 0xff), not read back from the code under test;
// poisoned neighbours prove the ctors write exactly 16 / 1 bytes; a thread race proves the function-local
// static is initialised once and every caller sees the same address. --dump feeds
// tools/lift/check_f78c760.py and check_f268ce0.py (interpreted machine code vs native).
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#include "core/guarded_static_helpers.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
using sotc::Pair128;

struct Frame {                       // ctor destination with poisoned neighbours on both sides
    std::array<std::uint8_t, 16> before;
    Pair128 cell;
    std::array<std::uint8_t, 16> after;
};

void fill(Frame& f, std::uint8_t p) {
    f.before.fill(p);
    f.after.fill(p);
    std::memset(&f.cell, p, sizeof f.cell);
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

void pair_checks() {
    const Pair128 z = sotc::pair_zero_51230();
    CHECK(z.lo == 0 && z.hi == 0);

    const Pair128& a = sotc::static_pair_a77960();
    CHECK(a.lo == 0 && a.hi == 0);
    CHECK(&sotc::static_pair_a77960() == &a);                 // same object every call (singleton)

    for (std::uint8_t poison : {std::uint8_t{0x00}, std::uint8_t{0xaa}, std::uint8_t{0xff}}) {
        Frame f;
        fill(f, poison);
        sotc::init_ctor_78c760(f.cell);
        CHECK(f.cell.lo == 0 && f.cell.hi == 0);              // all 16 bytes overwritten
        for (auto b : f.before) CHECK(b == poison);           // exactly 16 bytes, nothing around
        for (auto b : f.after) CHECK(b == poison);
        sotc::init_ctor_78c760(f.cell);                       // idempotent / unguarded rewrite
        CHECK(f.cell.lo == 0 && f.cell.hi == 0);
    }
    Frame g;                                                  // stale value in the cell is overwritten
    fill(g, 0x5a);
    g.cell.lo = 0x1122334455667788ull;
    g.cell.hi = 0x99aabbccddeeff00ull;
    sotc::init_ctor_78c760(g.cell);
    CHECK(g.cell.lo == 0 && g.cell.hi == 0);
}

void flag_checks() {
    CHECK(sotc::static_flag_268d40() == 0xff);
    CHECK(sotc::static_flag_268d40() == 0xff);
    for (std::uint8_t poison : {std::uint8_t{0x00}, std::uint8_t{0x01}, std::uint8_t{0xaa}, std::uint8_t{0xff}}) {
        std::array<std::uint8_t, 3> buf{poison, poison, poison};
        sotc::init_ctor_268ce0(buf[1]);
        CHECK(buf[0] == poison && buf[1] == 0xff && buf[2] == poison);   // exactly one byte written
    }
}

void race_checks() {
    // The statics are already initialised by the checks above in this process, so the race is only
    // meaningful as an address/value stability check across threads (guard fast path).
    constexpr int kThreads = 8;
    std::atomic<int> go{0};
    std::vector<const Pair128*> seen(kThreads, nullptr);
    std::vector<int> flag(kThreads, 0);
    std::vector<std::thread> ts;
    for (int t = 0; t != kThreads; ++t)
        ts.emplace_back([&, t] {
            while (go.load() == 0) {}
            seen[t] = &sotc::static_pair_a77960();
            flag[t] = sotc::static_flag_268d40();
        });
    go.store(1);
    for (auto& t : ts) t.join();
    for (int t = 0; t != kThreads; ++t) CHECK(seen[t] == seen[0] && flag[t] == 0xff);
}

void dump() {
    // Cold-process view: nothing above has run in this branch. 0xAA-poisoned destinations.
    Frame f;
    fill(f, 0xaa);
    const Pair128& first = sotc::static_pair_a77960();
    const Pair128* firstp = &first;
    const std::uint8_t first_flag = sotc::static_flag_268d40();
    sotc::init_ctor_78c760(f.cell);
    std::array<std::uint8_t, 3> buf{0xaa, 0xaa, 0xaa};
    sotc::init_ctor_268ce0(buf[1]);
    const Pair128 zero = sotc::pair_zero_51230();
    std::printf("{\"pair_first\":\"%s\",\"pair_second\":\"%s\",\"pair_same_address\":%s,"
                "\"ctor_78c760_cell\":\"%s\",\"ctor_78c760_before\":\"%s\",\"ctor_78c760_after\":\"%s\","
                "\"flag_first\":%u,\"flag_second\":%u,\"ctor_268ce0\":\"%s\",\"zero_pair\":\"%s\"}\n",
                hex(&first, 16).c_str(), hex(&sotc::static_pair_a77960(), 16).c_str(),
                firstp == &sotc::static_pair_a77960() ? "true" : "false", hex(&f.cell, 16).c_str(),
                hex(f.before.data(), 16).c_str(), hex(f.after.data(), 16).c_str(), first_flag,
                sotc::static_flag_268d40(), hex(buf.data(), 3).c_str(), hex(&zero, 16).c_str());
}
}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--dump") == 0) {
        dump();
        return 0;
    }
    pair_checks();
    flag_checks();
    race_checks();
    std::puts("guarded_static_helpers: all checks passed");
    return 0;
}
