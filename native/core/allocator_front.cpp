#include "allocator_front.hpp"

#include <cstdio>

namespace sotc {
namespace {

struct ScopeStack {
    std::uint32_t ids[AllocatorFront::kScopeDepth];
    std::uint32_t n = 0;
};
thread_local ScopeStack t_scope;

void default_report(void*, const OomInfo& info) {
    std::fprintf(stderr, "sotc: out of memory: %zu bytes requested from heap %u\n", info.size,
                 info.heap_id);
}
void default_trap(void*, const OomInfo&) { __builtin_trap(); }

}  // namespace

bool AllocatorFront::set_heap(std::uint32_t id, IAllocator* heap) noexcept {
    if (id == 0 || id >= kHeapSlots || heap == nullptr) return false;
    table_[id].store(heap);
    return true;
}

bool AllocatorFront::set_heap_if_empty(std::uint32_t id, IAllocator* heap) noexcept {
    if (id == 0 || id >= kHeapSlots || heap == nullptr) return false;
    IAllocator* empty = nullptr;
    return table_[id].compare_exchange_strong(empty, heap);
}

bool AllocatorFront::clear_heap_if(std::uint32_t id, IAllocator* expected) noexcept {
    if (id == 0 || id >= kHeapSlots || expected == nullptr) return false;
    return table_[id].compare_exchange_strong(expected, nullptr);
}

void AllocatorFront::clear_heap(std::uint32_t id) noexcept {
    if (id < kHeapSlots) table_[id].store(nullptr);
}

IAllocator* AllocatorFront::heap(std::uint32_t id) const noexcept {
    return id < kHeapSlots ? table_[id].load() : nullptr;
}

void AllocatorFront::set_oom_hooks(const OomHooks& hooks) {
    std::lock_guard<std::mutex> lock(hooks_mu_);
    hooks_ = hooks;
}

void AllocatorFront::set_lazy_bootstrap(const LazyBootstrapHooks& hooks) {
    std::lock_guard<std::mutex> lock(hooks_mu_);
    lazy_bootstrap_ = hooks;
}

bool AllocatorFront::maybe_lazy_bootstrap(std::uint32_t& heap_id) {
    // OBS 0x1284a: if latch byte != 0 → skip. OBS 0x12854: if tbl[1] != 0 → skip.
    if (bootstrap_latch_.load(std::memory_order_relaxed)) return false;
    if (heap(kDefaultHeap) != nullptr) return false;

    LazyBootstrapHooks lb;
    {
        std::lock_guard<std::mutex> lock(hooks_mu_);
        lb = lazy_bootstrap_;
    }
    if (lb.install == nullptr) return false;
    if (!lb.install(*this, lb.user)) return false;
    // OBS 0x12865: heap id := returned[+0x1c]; empty-table composite lives in slot 1.
    heap_id = kDefaultHeap;
    return true;
}

void AllocatorFront::report_oom(const OomInfo& info) {
    OomHooks h;
    {
        std::lock_guard<std::mutex> lock(hooks_mu_);
        h = hooks_;
    }
    // OBS latch: the report runs once. INF: the trap runs for every unrecovered failure.
    if (!oom_latch_.exchange(true)) {
        oom_reports_.fetch_add(1);
        (h.report ? h.report : default_report)(h.user, info);
    }
    (h.trap ? h.trap : default_trap)(h.user, info);
}

void* AllocatorFront::alloc(std::size_t size, std::uint32_t heap_id, std::size_t align,
                            std::uint32_t flags) {
    // OBS 0x1284a..0x1286f before scoped-heap / table lookup.
    (void)maybe_lazy_bootstrap(heap_id);

    // OBS: the scoped override applies only to heap 1 and only with a non-empty stack.
    if (heap_id == kDefaultHeap) {
        const std::uint32_t top = ScopedHeap::top();
        if (top != 0) heap_id = top;
    }
    IAllocator* h = heap(heap_id);
    if (h == nullptr) return nullptr;  // OBS: missing heap -> NULL, silently
    if (size == 0) size = 1;           // OBS: cmovne
    const AllocDesc desc{flags, tag_.load(std::memory_order_relaxed)};
    void* p = h->allocate(size, align, desc);
    if (p != nullptr) return p;
    if (h->recover_from_failure()) return nullptr;  // OBS vt[+0x98] true: no report
    report_oom(OomInfo{size, heap_id, h});
    return nullptr;  // reached only if the trap hook returns (never with the default hook)
}

void AllocatorFront::free(void* ptr, std::uint32_t kind) {
    if (ptr == nullptr) return;                    // OBS: free(NULL) is a no-op
    const auto owner = owners_.lookup(ptr);
    if (!owner) return;                            // OBS: foreign pointer ignored
    IAllocator* h = heap(*owner);
    if (h == nullptr) return;                      // INF: owner slot cleared -> ignore
    IAllocator* default_heap = heap(kDefaultHeap);
    if (default_heap != nullptr && default_heap != h)
        default_heap->on_owner_map_deallocation(ptr);
    h->deallocate(ptr, AllocDesc{kind, tag_.load(std::memory_order_relaxed)});
}

ScopedHeap::ScopedHeap(std::uint32_t heap_id) noexcept {
    if (t_scope.n < AllocatorFront::kScopeDepth) {
        t_scope.ids[t_scope.n++] = heap_id;
        pushed_ = true;
    }
}

ScopedHeap::~ScopedHeap() {
    if (pushed_) --t_scope.n;
}

std::uint32_t ScopedHeap::top() noexcept {
    return t_scope.n == 0 ? 0 : t_scope.ids[t_scope.n - 1];
}

AllocatorFront& global_allocator_front() noexcept {
    static AllocatorFront front;
    return front;
}

}  // namespace sotc
