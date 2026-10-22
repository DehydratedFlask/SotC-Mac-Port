// 0x14c9a0 (parent == nullptr), 0x14d880 and 0xb25470 (native/core/registry_object_14c9a0).
// Expected values are literals taken from the documented stores (not read back from the code under test);
// 0xAA-poisoned objects prove exactly which bytes the ctor writes; the parent != nullptr contract is
// verified in a forked child that must die with SIGABRT before touching the object (a pipe-less check:
// the object's memory is shared through MAP_SHARED so the parent can inspect it afterwards).
// --dump feeds tools/lift/check_f14c9a0.py, check_f14d880.py and check_fb25470.py.
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <array>
#include <memory>
#include <atomic>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#include "core/registry_object_14c9a0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
using sotc::HashedStr;
using sotc::Object14c9a0;
using sotc::Slot118;

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
// hex with the listed 8-byte slots zeroed (they hold native addresses)
std::string masked(const void* d, std::size_t n, std::initializer_list<std::size_t> slots) {
    std::vector<std::uint8_t> v(static_cast<const std::uint8_t*>(d), static_cast<const std::uint8_t*>(d) + n);
    for (auto s : slots) std::memset(v.data() + s, 0, 8);
    return hex(v.data(), n);
}
long long rel(const void* p, const void* base) { return static_cast<const char*>(p) - static_cast<const char*>(base); }

void check_static_hashed() {
    const HashedStr& hs = sotc::static_empty_hashed_2ed3640();
    CHECK(&sotc::static_empty_hashed_2ed3640() == &hs);
    CHECK(hs.hash == 0x811c9dc5u);                              // FNV offset basis: empty span
    CHECK(all(hs.gap04, 4, 0));                                 // BSS padding, never written
    CHECK(hs.str.data == sotc::empty_string_f730().data && hs.str.size == 0 && hs.str.aux == 0 && hs.str.ctl == nullptr);
}

void check_ctor(std::uint8_t poison) {
    alignas(16) Object14c9a0 o;
    std::memset(&o, poison, sizeof o);
    sotc::ctor_14c9a0(o, nullptr);
    CHECK(o.q00 == 0 && o.self08 == &o && o.self10 == &o && o.q18 == 0 && o.parent == nullptr);
    CHECK(all(o.gap28, 8, poison));
    CHECK(o.a30 == 0 && all(o.gap34, 12, poison) && o.a40 == 0 && all(o.gap44, 12, poison) && o.a50 == 0);
    CHECK(all(o.gap58, 8, poison) && o.d60 == 0 && all(o.gap64, 4, poison) && o.q68 == 0);
    for (const HashedStr* h : {&o.s70, &o.sc8}) {
        CHECK(h->hash == 0x811c9dc5u && all(h->gap04, 4, poison));          // gap not copied
        CHECK(h->str.data == sotc::empty_string_f730().data && h->str.size == 0 && h->str.aux == 0 && h->str.ctl == nullptr);
    }
    CHECK(o.d98 == 0xb && all(o.gap9c, 12, poison));
    CHECK(all(o.zero_a8, 32, 0));
    CHECK(o.df0 == 0 && o.df4 == 0xb && o.bf8 == 0 && all(o.gapf9, 7, poison));
}

// parent != nullptr: forked child must abort (SIGABRT) and must not have written to the shared object.
void check_unsupported_parent() {
    void* mem = mmap(nullptr, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    CHECK(mem != MAP_FAILED);
    auto* o = static_cast<Object14c9a0*>(mem);
    std::memset(o, 0xAA, sizeof *o);
    const pid_t pid = fork();
    CHECK(pid >= 0);
    if (pid == 0) {
        // keep the diagnostic out of the test log
        if (freopen("/dev/null", "w", stderr) == nullptr) _exit(3);
        Object14c9a0* fake_parent = o + 1;   // any non-null pointer; never dereferenced by the unsupported contract
        sotc::ctor_14c9a0(*o, fake_parent);
        _exit(0);                            // reaching here means the contract was silently modelled
    }
    int status = 0;
    CHECK(waitpid(pid, &status, 0) == pid);
    CHECK(WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT);
    CHECK(all(static_cast<const std::uint8_t*>(mem), sizeof(Object14c9a0), 0xAA));   // nothing written first
    munmap(mem, 4096);
}

void check_b25470_cold_then_getter() {
    // Cold process state: cell null, guard unset, counter zero-initialised.
    CHECK(sotc::registry_cell_2ee3b00().load() == nullptr);
    CHECK(sotc::static_object_2ee3b10().a30 == 0);
    auto slots = std::make_unique<std::array<Slot118, sotc::kSlotCountB25470>>();
    std::memset(slots.get(), 0xAA, sizeof *slots);
    sotc::init_ctor_b25470(*slots);
    Object14c9a0& obj = sotc::static_object_2ee3b10();
    CHECK(sotc::registry_cell_2ee3b00().load() == &obj);
    // counter: ctor zeroes it, the first getter call increments (1), then add_ref on each of the 400 iterations.
    CHECK(obj.a30 == 401);
    CHECK(obj.self08 == &obj && obj.d98 == 0xb && obj.df4 == 0xb);       // ctor ran exactly once, parent null
    for (const Slot118& s : *slots) {
        CHECK(s.flag00 == 0 && s.flag110 == 0 && s.node == &obj);
        CHECK(all(s.gap01, 0x107, 0xAA) && all(s.gap111, 7, 0xAA));        // untouched bytes
    }
    static_assert(sizeof(*slots) == 0x1b580);

    // getter: cell non-null -> returns it without touching the counter
    CHECK(sotc::getter_14d880() == &obj && obj.a30 == 401);
    // cell found null again (guard already set): no ctor re-run, counter incremented
    obj.d60 = 0x77;
    sotc::registry_cell_2ee3b00().store(nullptr);
    CHECK(sotc::getter_14d880() == &obj);
    CHECK(obj.a30 == 402 && obj.d60 == 0x77 && sotc::registry_cell_2ee3b00().load() == &obj);
    // cell holding some other pointer: returned as-is, nothing changes
    Object14c9a0 other{};
    sotc::registry_cell_2ee3b00().store(&other);
    CHECK(sotc::getter_14d880() == &other && obj.a30 == 402);
    sotc::registry_cell_2ee3b00().store(&obj);

    // race: 8 threads hammering the getter keep the single object
    std::atomic<int> go{0};
    std::vector<Object14c9a0*> seen(8, nullptr);
    std::vector<std::thread> ts;
    for (int t = 0; t != 8; ++t) ts.emplace_back([&, t] { while (!go.load()) {} seen[t] = sotc::getter_14d880(); });
    go.store(1);
    for (auto& t : ts) t.join();
    for (auto* p : seen) CHECK(p == &obj);
    CHECK(obj.a30 == 402);
}

void dump() {
    // Cold sequence 1: poisoned/zero local objects (ctor only).
    std::printf("{");
    for (std::uint8_t bg : {std::uint8_t{0xAA}, std::uint8_t{0x00}}) {
        alignas(16) Object14c9a0 o;
        std::memset(&o, bg, sizeof o);
        sotc::ctor_14c9a0(o, nullptr);
        const auto& e = sotc::empty_string_f730();
        std::printf("\"ctor_%02x\":{\"bytes\":\"%s\",\"self08\":%lld,\"self10\":%lld,\"p78\":%lld,\"pd0\":%lld},", bg,
                    masked(&o, sizeof o, {0x08, 0x10, 0x78, 0xd0}).c_str(), rel(o.self08, &o), rel(o.self10, &o),
                    rel(o.s70.str.data, &e), rel(o.sc8.str.data, &e));
    }
    const auto& hs = sotc::static_empty_hashed_2ed3640();
    const auto& e = sotc::empty_string_f730();
    std::printf("\"hs\":{\"bytes\":\"%s\",\"p\":%lld},\"empty\":{\"bytes\":\"%s\",\"p\":%lld},", masked(&hs, sizeof hs, {0x08}).c_str(),
                rel(hs.str.data, &e), masked(&e, sizeof e, {0x00}).c_str(), rel(e.data, &e));
    // Cold sequence 2 is not possible after the ctor above only for statics that were already built
    // (guards), but the object/cell/counter statics have not been touched yet: run 0xb25470 now.
    auto slots = std::make_unique<std::array<Slot118, sotc::kSlotCountB25470>>();
    std::memset(slots.get(), 0xAA, sizeof *slots);
    sotc::init_ctor_b25470(*slots);
    Object14c9a0& obj = sotc::static_object_2ee3b10();
    bool same = true, nodes = true;
    const std::string first = masked(&(*slots)[0], sizeof(Slot118), {0x108});
    for (const Slot118& s : *slots) {
        if (masked(&s, sizeof s, {0x108}) != first) same = false;
        if (s.node != &obj) nodes = false;
    }
    std::printf("\"b25470\":{\"counter\":%d,\"slot0\":\"%s\",\"slots_identical\":%s,\"nodes_all_object\":%s,\"cell_is_object\":%s,"
                "\"object\":\"%s\",\"self08\":%lld,\"p78\":%lld,\"pd0\":%lld,\"count\":%zu,\"stride\":%zu},",
                obj.a30, first.c_str(), same ? "true" : "false", nodes ? "true" : "false",
                sotc::registry_cell_2ee3b00().load() == &obj ? "true" : "false", masked(&obj, sizeof obj, {0x08, 0x10, 0x78, 0xd0}).c_str(),
                rel(obj.self08, &obj), rel(obj.s70.str.data, &e), rel(obj.sc8.str.data, &e), slots->size(), sizeof(Slot118));
    // getter: early return / null-cell re-entry
    const int c0 = obj.a30;
    Object14c9a0* r1 = sotc::getter_14d880();
    const int c1 = obj.a30;
    sotc::registry_cell_2ee3b00().store(nullptr);
    Object14c9a0* r2 = sotc::getter_14d880();
    const int c2 = obj.a30;
    std::printf("\"getter\":{\"c0\":%d,\"r1_obj\":%s,\"c1\":%d,\"r2_obj\":%s,\"c2\":%d}}\n", c0, r1 == &obj ? "true" : "false", c1,
                r2 == &obj ? "true" : "false", c2);
}
}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--dump") == 0) { dump(); return 0; }
    // Order matters: the b25470/getter test needs the object/cell statics untouched (cold).
    check_b25470_cold_then_getter();
    check_static_hashed();
    check_ctor(0xAA);
    check_ctor(0x00);
    check_ctor(0xFF);
    check_unsupported_parent();
    std::puts("registry_object_14c9a0: all checks passed");
    return 0;
}
