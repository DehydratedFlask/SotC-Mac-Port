#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <thread>
#include <vector>

#include "core/atomic_add_ref_14ccb0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

constexpr std::size_t kBuf = 0x80;

struct alignas(16) Buffer { unsigned char b[kBuf]; };

std::int32_t cell_of(const Buffer& o) { std::int32_t v; std::memcpy(&v, o.b + 0x30, 4); return v; }
void set_cell(Buffer& o, std::int32_t v) { std::memcpy(o.b + 0x30, &v, 4); }

void dump() {
    const std::int32_t inits[] = {0, 1, -1, 2, 0x7ffffffe, std::numeric_limits<std::int32_t>::max(),
                                  std::numeric_limits<std::int32_t>::min(), 0x12345678, -2};
    std::printf("{\"rva\":%u,\"body_length\":5,\"offset\":%u,\"buffer\":%zu,\"cases\":[", 0x14ccb0u,
                static_cast<unsigned>(sotc::kAddRef14ccb0Offset), kBuf);
    bool first = true;
    for (std::int32_t v : inits) {
        Buffer o;
        std::memset(o.b, 0xAA, sizeof o.b);   // poison: any byte outside +0x30..+0x33 must stay 0xAA
        set_cell(o, v);
        sotc::add_ref_14ccb0(static_cast<void*>(o.b));
        std::printf("%s{\"init\":%d,\"bytes\":\"", first ? "" : ",", v);
        for (unsigned char c : o.b) std::printf("%02x", c);
        std::printf("\"}");
        first = false;
    }
    std::printf("]}\n");
}

}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--dump") { dump(); return 0; }

    // Typed overload.
    std::atomic<std::int32_t> a{0};
    sotc::add_ref_14ccb0(a);
    CHECK(a.load() == 1);
    sotc::add_ref_14ccb0(a);
    CHECK(a.load() == 2);
    a.store(-1);
    sotc::add_ref_14ccb0(a);
    CHECK(a.load() == 0);                                          // crosses zero
    a.store(std::numeric_limits<std::int32_t>::max());
    sotc::add_ref_14ccb0(a);
    CHECK(a.load() == std::numeric_limits<std::int32_t>::min());   // wraps like lock inc

    // Object overload: only the dword at +0x30 changes, nothing else is read-modified-written.
    Buffer o;
    std::memset(o.b, 0x5C, sizeof o.b);
    set_cell(o, 41);
    Buffer before = o;
    sotc::add_ref_14ccb0(static_cast<void*>(o.b));
    CHECK(cell_of(o) == 42);
    for (std::size_t i = 0; i < kBuf; ++i)
        if (i < 0x30 || i >= 0x34) CHECK(o.b[i] == before.b[i]);
    set_cell(o, std::numeric_limits<std::int32_t>::max());
    sotc::add_ref_14ccb0(static_cast<void*>(o.b));
    CHECK(cell_of(o) == std::numeric_limits<std::int32_t>::min());

    // Atomicity: two threads, no lost updates (a plain ++ would lose many with this count).
    constexpr int kPer = 200000;
    {
        Buffer shared;
        std::memset(shared.b, 0, sizeof shared.b);
        std::atomic<bool> go{false};
        auto work = [&] { while (!go.load()) {} for (int i = 0; i < kPer; ++i) sotc::add_ref_14ccb0(static_cast<void*>(shared.b)); };
        std::thread t1(work), t2(work);
        go = true;
        t1.join(); t2.join();
        CHECK(cell_of(shared) == 2 * kPer);
    }
    // Same through the typed overload with four threads.
    {
        std::atomic<std::int32_t> shared{0};
        std::atomic<bool> go{false};
        std::vector<std::thread> ts;
        for (int k = 0; k < 4; ++k)
            ts.emplace_back([&] { while (!go.load()) {} for (int i = 0; i < kPer; ++i) sotc::add_ref_14ccb0(shared); });
        go = true;
        for (auto& t : ts) t.join();
        CHECK(shared.load() == 4 * kPer);
    }

    std::puts("atomic_add_ref_14ccb0: all checks passed");
    return 0;
}
