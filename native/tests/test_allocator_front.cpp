// Front-end contract tests (0x12820 alloc / 0x12af0 free / 0x17070.. wrappers) against fakes and
// the real PageAllocator. See allocator_front.hpp for OBS/INF labels.
#include <sys/wait.h>
#include <unistd.h>

#include <atomic>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <thread>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/page_allocator.hpp"

#define CHECK(c)                                                              \
    do {                                                                      \
        if (!(c)) {                                                           \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);  \
            std::exit(1);                                                     \
        }                                                                     \
    } while (0)

using namespace sotc;

// Fake heap: malloc-backed, fills with 0xA5 (to prove the front-end never zero-fills), records
// calls, can be told to fail, registers its blocks in the owner map like a real layer would.
struct FakeHeap : IAllocator {
    FakeHeap(PageOwnerMap& o, std::uint32_t id) : owners(o), id(id) {}
    ~FakeHeap() override {
        for (void* p : live) { owners.remove(reinterpret_cast<std::uintptr_t>(p)); std::free(p); }
    }
    void* allocate(std::size_t size, std::size_t align, const AllocDesc& d) override {
        ++alloc_calls; last_size = size; last_align = align; last_desc = d;
        if (fail) return nullptr;
        void* p = nullptr;
        if (posix_memalign(&p, align < sizeof(void*) ? sizeof(void*) : align, size) != 0) return nullptr;
        std::memset(p, 0xA5, size);
        CHECK(owners.add(reinterpret_cast<std::uintptr_t>(p), size, id) == PageOwnerMap::Status::ok);
        live.push_back(p);
        return p;
    }
    void deallocate(void* p, const AllocDesc& d) override {
        ++free_calls; last_free = p; last_free_desc = d;
        for (std::size_t i = 0; i < live.size(); ++i)
            if (live[i] == p) {
                owners.remove(reinterpret_cast<std::uintptr_t>(p));
                std::free(p);
                live.erase(live.begin() + static_cast<std::ptrdiff_t>(i));
                return;
            }
        CHECK(false);  // front-end must only forward owned pointers
    }
    bool recover_from_failure() override { ++recover_calls; return recover_result; }

    PageOwnerMap& owners;
    std::uint32_t id;
    bool fail = false, recover_result = false;
    int alloc_calls = 0, free_calls = 0, recover_calls = 0;
    std::size_t last_size = 0, last_align = 0;
    AllocDesc last_desc, last_free_desc;
    void* last_free = nullptr;
    std::vector<void*> live;
};

struct OomLog {
    int reports = 0, traps = 0;
    OomInfo last_report{}, last_trap{};
};
static void on_report(void* u, const OomInfo& i) { auto* l = static_cast<OomLog*>(u); ++l->reports; l->last_report = i; }
static void on_trap(void* u, const OomInfo& i) { auto* l = static_cast<OomLog*>(u); ++l->traps; l->last_trap = i; }

static void test_alloc_contract() {
    AllocatorFront f;
    FakeHeap h(f.owners(), 3);
    CHECK(f.set_heap(3, &h));
    CHECK(!f.set_heap(0, &h));       // slot 0 never populated
    CHECK(!f.set_heap(256, &h));
    CHECK(!f.set_heap(4, nullptr));

    // size 0 -> 1, align and flags forwarded, tag forwarded, memory untouched (no zero fill).
    f.set_tag(0x77);
    auto* p = static_cast<unsigned char*>(f.alloc(0, 3, 64, 5));
    CHECK(p != nullptr);
    CHECK(h.last_size == 1 && h.last_align == 64);
    CHECK(h.last_desc.flags == 5 && h.last_desc.tag == 0x77);
    CHECK(p[0] == 0xA5);
    CHECK(reinterpret_cast<std::uintptr_t>(p) % 64 == 0);
    p = static_cast<unsigned char*>(f.alloc(4096, 3, 8, 1));
    CHECK(h.last_size == 4096 && p[0] == 0xA5 && p[4095] == 0xA5);

    // Missing heap: nullptr, no report, no trap.
    OomLog log;
    f.set_oom_hooks({on_report, on_trap, &log});
    CHECK(f.alloc(16, 9, 8, 1) == nullptr);
    CHECK(f.alloc(16, 0, 8, 1) == nullptr);
    CHECK(f.alloc(16, 256, 8, 1) == nullptr);
    CHECK(f.alloc(16, 0xffffffffu, 8, 1) == nullptr);
    CHECK(log.reports == 0 && log.traps == 0);
    CHECK(h.alloc_calls == 2);

    // wrappers target heap 1 / align 8 / flags 1 and 2.
    FakeHeap def(f.owners(), 2);
    CHECK(f.set_heap(1, &def));
    void* a = f.new_(10);
    CHECK(a && def.last_align == 8 && def.last_desc.flags == 1 && def.last_size == 10);
    void* b = f.new_array(0);
    CHECK(b && def.last_desc.flags == 2 && def.last_size == 1);
    CHECK(f.owners().lookup(a) == 2u);  // fake registered under id 2
    // delete / delete[] -> kind 1 / 2 delivered to the owner heap (registered at slot 2)
    CHECK(f.set_heap(2, &def));
    f.delete_(a);
    CHECK(def.free_calls == 1 && def.last_free == a && def.last_free_desc.flags == 1);
    f.delete_array(b);
    CHECK(def.free_calls == 2 && def.last_free_desc.flags == 2);

    // free(nullptr) and foreign pointers: ignored, nothing forwarded.
    f.delete_(nullptr);
    int local = 0;
    f.delete_(&local);
    f.free(reinterpret_cast<void*>(0x10), 1);
    CHECK(def.free_calls == 2 && h.free_calls == 0);
    // interior pointer of an owned block still resolves to its owner (INF design)
    f.free(p, 1);
    CHECK(h.free_calls == 1);
}

static void test_oom() {
    AllocatorFront f;
    FakeHeap h(f.owners(), 4);
    f.set_heap(4, &h);
    OomLog log;
    f.set_oom_hooks({on_report, on_trap, &log});

    h.fail = true;
    CHECK(f.alloc(0, 4, 8, 1) == nullptr);  // size 0 reported as 1
    CHECK(h.recover_calls == 1);
    CHECK(log.reports == 1 && log.traps == 1);
    CHECK(log.last_report.size == 1 && log.last_report.heap_id == 4 && log.last_report.heap == &h);
    CHECK(f.oom_report_count() == 1);
    // reported once; trap still fires for every unrecovered failure (INF)
    CHECK(f.alloc(99, 4, 8, 1) == nullptr);
    CHECK(log.reports == 1 && log.traps == 2 && log.last_trap.size == 99);
    CHECK(f.oom_report_count() == 1);

    // recover_from_failure()==true suppresses both report and trap (OBS vt+0x98).
    AllocatorFront g;
    FakeHeap h2(g.owners(), 5);
    g.set_heap(5, &h2);
    OomLog log2;
    g.set_oom_hooks({on_report, on_trap, &log2});
    h2.fail = true;
    h2.recover_result = true;
    CHECK(g.alloc(8, 5, 8, 1) == nullptr);
    CHECK(h2.recover_calls == 1 && log2.reports == 0 && log2.traps == 0);
    // a success never consults the hook
    h2.fail = false;
    void* ok = g.alloc(8, 5, 8, 1);
    CHECK(ok && h2.recover_calls == 1);
    g.free(ok, 1);

    // Default report with an injected trap (stderr gets one native line).
    AllocatorFront d;
    FakeHeap h3(d.owners(), 6);
    d.set_heap(6, &h3);
    h3.fail = true;
    OomLog log3;
    d.set_oom_hooks({nullptr, on_trap, &log3});
    CHECK(d.alloc(7, 6, 8, 1) == nullptr);
    CHECK(log3.traps == 1 && d.oom_report_count() == 1);
}

// Default hooks must trap (process killed by a signal), never return silently.
static void test_default_trap() {
    std::fflush(nullptr);
    pid_t pid = fork();
    CHECK(pid >= 0);
    if (pid == 0) {
        AllocatorFront f;
        FakeHeap h(f.owners(), 4);
        f.set_heap(4, &h);
        h.fail = true;
        (void)f.alloc(8, 4, 8, 1);
        _exit(0);  // reaching here means no trap
    }
    int st = 0;
    CHECK(waitpid(pid, &st, 0) == pid);
    CHECK(WIFSIGNALED(st));
    const int sig = WTERMSIG(st);
    CHECK(sig == SIGILL || sig == SIGTRAP || sig == SIGABRT);
}

static void test_scoped_heap() {
    AllocatorFront f;
    FakeHeap def(f.owners(), 2), scoped(f.owners(), 3), other(f.owners(), 4);
    f.set_heap(1, &def);
    f.set_heap(3, &scoped);
    f.set_heap(4, &other);
    CHECK(ScopedHeap::top() == 0);
    {
        ScopedHeap s(3);
        CHECK(s.active() && ScopedHeap::top() == 3);
        void* p = f.new_(8);                 // heap 1 -> overridden
        CHECK(scoped.alloc_calls == 1 && def.alloc_calls == 0);
        void* q = f.alloc(8, 4, 8, 1);       // explicit non-1 heap is NOT overridden
        CHECK(other.alloc_calls == 1 && scoped.alloc_calls == 1);
        {
            ScopedHeap s2(4);
            (void)f.new_(8);
            CHECK(other.alloc_calls == 2);
        }
        CHECK(ScopedHeap::top() == 3);
        f.free(p, 1);
        f.free(q, 1);
    }
    CHECK(ScopedHeap::top() == 0);
    void* r = f.new_(8);
    CHECK(def.alloc_calls == 1);
    f.set_heap(2, &def);
    f.delete_(r);

    // depth limit 32
    std::vector<std::unique_ptr<ScopedHeap>> st;
    for (std::uint32_t i = 0; i < AllocatorFront::kScopeDepth; ++i) {
        st.push_back(std::make_unique<ScopedHeap>(3));
        CHECK(st.back()->active());
    }
    ScopedHeap over(4);
    CHECK(!over.active() && ScopedHeap::top() == 3);
    st.clear();
    CHECK(ScopedHeap::top() == 0);
    // another thread does not see this thread's scope
    ScopedHeap mine(3);
    std::uint32_t seen = 99;
    std::thread([&] { seen = ScopedHeap::top(); }).join();
    CHECK(seen == 0);
}

static void test_with_page_allocator() {
    AllocatorFront f;
    PageAllocator pa(f.owners(), 2);
    CHECK(f.set_heap(1, &pa));  // default heap = page layer
    CHECK(f.set_heap(2, &pa));  // and as its own owner slot
    auto* p = static_cast<unsigned char*>(f.new_(1));
    CHECK(p && pa.owns_block(p));
    CHECK(reinterpret_cast<std::uintptr_t>(p) % 0x10000 == 0);
    CHECK(f.owners().lookup(p) == 2u);
    std::memset(p, 1, 0x10000);
    f.delete_(p);
    CHECK(!pa.owns_block(p) && pa.stats().live_blocks == 0);
    f.delete_(p);  // double free: owner lookup now fails -> ignored, no crash
    CHECK(pa.stats().ignored_frees == 0);
    // unmapped memory, rather than a stale record, answers "foreign"
    CHECK(f.owners().lookup(p) == std::nullopt);

    // Aligned request honoured; failing request (align not pow2 after rounding) hits OOM path.
    OomLog log;
    f.set_oom_hooks({on_report, on_trap, &log});
    void* a = f.alloc(0x20000, 1, 0x400000, 1);
    CHECK(a && reinterpret_cast<std::uintptr_t>(a) % 0x400000 == 0);
    f.free(a, 1);
    CHECK(f.alloc(0x10000, 1, 0x300000, 1) == nullptr);
    CHECK(log.reports == 1 && log.traps == 1);
}

static void test_global_wrappers() {
    AllocatorFront& g = global_allocator_front();
    CHECK(game_new(8) == nullptr);  // nothing installed: silent null
    FakeHeap h(g.owners(), 2);
    CHECK(g.set_heap(1, &h) && g.set_heap(2, &h));
    void* p = game_new(24);
    void* q = game_new_array(24);
    CHECK(p && q && h.live.size() == 2);
    game_delete(p);
    game_delete_array(q);
    CHECK(h.live.empty() && h.free_calls == 2);
    g.clear_heap(1);
    g.clear_heap(2);
    CHECK(g.heap(1) == nullptr && g.heap(300) == nullptr);
}

static void test_threads() {
    AllocatorFront f;
    PageAllocator pa(f.owners(), 2);
    f.set_heap(1, &pa);
    f.set_heap(2, &pa);
    std::atomic<int> bad{0};
    std::vector<std::thread> ts;
    for (int t = 0; t < 4; ++t)
        ts.emplace_back([&, t] {
            for (int i = 0; i < 30; ++i) {
                auto* p = static_cast<unsigned char*>(f.new_(100000 + t));
                if (!p) { ++bad; continue; }
                std::memset(p, t + 1, 100000);
                if (p[99999] != t + 1) ++bad;
                f.delete_(p);
            }
        });
    for (auto& th : ts) th.join();
    CHECK(bad == 0 && pa.stats().live_blocks == 0 && f.owners().range_count() == 0);
}

int main() {
    test_alloc_contract();
    test_oom();
    test_default_trap();
    test_scoped_heap();
    test_with_page_allocator();
    test_global_wrappers();
    test_threads();
    std::puts("allocator_front OK");
    return 0;
}
