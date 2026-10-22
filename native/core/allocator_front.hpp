// SRV-01: allocator FRONT-END contract (original RVAs 0x12820 alloc, 0x12af0 free, wrappers
// 0x17070 new / 0x17090 new[] / 0x170b0 delete / 0x170c0 delete[]).
//
// This is a CONTRACT implementation derived from the private disassembly write-up, not a
// byte-for-byte lift. Labels used below:
//   OBS = behaviour read from the shipped code (private write-up, evidence in local/validation)
//   INF = native design choice / inference; nothing proves the original does exactly this.
//
// OBS behaviours implemented:
//   * alloc(size, heap, align, flags): heap table of 256 slots, slot 0 is never populated.
//   * heap slot empty/null -> returns nullptr with NO out-of-memory report.
//   * size 0 -> 1 before the heap is called.
//   * descriptor {flags, tag} is forwarded to the heap; align is forwarded unmodified.
//   * result memory is NOT zero-filled (the front-end never writes the block).
//   * heap returned null -> heap->recover_from_failure(); if false -> OOM report, then return null.
//   * the OOM *report* is latched: it runs at most once per front-end (OBS latch byte 0x2dfb2b0).
//   * free(nullptr) is a no-op; a pointer outside every registered page range (foreign) is
//     silently ignored; otherwise the owner heap's deallocate(ptr, {kind}) is called.
//   * new / new[] = alloc(size, heap 1, align 8, flags 1 / 2); delete / delete[] = free kind 1 / 2.
//   * scoped-heap override: only when the requested heap is 1 and the calling thread has a
//     non-empty scope stack (<= 32 entries), the top entry replaces the heap id.
// INF behaviours (native policy):
//   * after the report the injectable trap hook runs on EVERY unrecovered failure (original traps
//     only when its assert-handler returns non-zero; the default native hook is __builtin_trap, so
//     in production the first failure ends the process). A returning trap hook (tests) makes alloc
//     return nullptr.
//   * heap ids >= 256 are treated as a missing heap (original index range unknown).
//   * the scope stack is one process-wide thread_local stack (original: per-thread block).
//   * Lazy bootstrap gate (OBS 0x1284a..0x1286f / byte 0x2dfaaa0): if latch==0 and tbl[1]==null,
//     invoke an injectable install hook (native stand-in for call 0x13380). On success the current
//     alloc's heap id becomes the bootstrap object's id (OBS [obj+0x1c]; empty-table → slot 1).
//     Latch itself is NOT set by 0x13380 (OBS); later allocs skip via tbl[1]!=null. Separate
//     init 0x110d3 sets latch=1; shutdown 0x11f94 clears latch=0 — exposed as set_bootstrap_latch.
//   * not modelled: TLS free-heap hint, realloc (0x12c50), get_heap (0x12a70), 5-arg 0x12950,
//     profiling tracker, free-path latch copies, object[+0x1c] field layout.
//   * Explicit bootstrap wiring helper: see allocator_bootstrap.hpp (OBS 0x13380).
//   * composite / page layers: see composite_allocator.hpp + page_allocator.hpp (install as heap 1).
// Thread safety: front-end is lock-free on the alloc path (OBS) apart from the cold OOM / lazy
// bootstrap hook path (hook + latch load under the same mutex as OOM hooks).
#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>

#include "iallocator.hpp"
#include "page_owner_map.hpp"

namespace sotc {

class AllocatorFront;

struct OomInfo {
    std::size_t size;        // after the 0 -> 1 adjustment
    std::uint32_t heap_id;   // effective heap id after scoped override
    const IAllocator* heap;
};

// Injectable out-of-memory hooks. Null members select the defaults:
//   report: one line on stderr (native text; the shipped wording is deliberately not copied);
//   trap:   __builtin_trap().
struct OomHooks {
    void (*report)(void* user, const OomInfo& info) = nullptr;
    void (*trap)(void* user, const OomInfo& info) = nullptr;
    void* user = nullptr;
};

// OBS 0x12860 call target stand-in: install default heaps into `front`. Return true on success
// (tbl[1] must be non-null afterwards). False → alloc continues and typically returns null.
using LazyBootstrapFn = bool (*)(AllocatorFront& front, void* user);

struct LazyBootstrapHooks {
    LazyBootstrapFn install = nullptr;
    void* user = nullptr;
};

class AllocatorFront {
public:
    static constexpr std::uint32_t kHeapSlots = 256;       // OBS table of 256 u64
    static constexpr std::uint32_t kDefaultHeap = 1;       // OBS wrappers use heap 1
    static constexpr std::size_t kDefaultAlign = 8;        // OBS wrappers use align 8
    static constexpr std::uint32_t kScopeDepth = 32;       // OBS 0x400-byte stack / 32-byte entries

    // Registers a heap in slot `id` (2..255 keeps the PageOwnerMap invariant; slot 1 is the
    // default composite heap; slot 0 is rejected). Returns false on a bad id / null heap.
    bool set_heap(std::uint32_t id, IAllocator* heap) noexcept;
    void clear_heap(std::uint32_t id) noexcept;
    IAllocator* heap(std::uint32_t id) const noexcept;

    PageOwnerMap& owners() noexcept { return owners_; }

    void set_oom_hooks(const OomHooks& hooks);
    std::uint32_t oom_report_count() const noexcept { return oom_reports_.load(); }
    void set_tag(std::uint32_t tag) noexcept { tag_.store(tag); }

    // OBS byte 0x2dfaaa0 (0 = allow lazy bootstrap check; 1 = skip). Default 0.
    bool bootstrap_latch() const noexcept { return bootstrap_latch_.load(); }
    void set_bootstrap_latch(bool set) noexcept { bootstrap_latch_.store(set); }

    // Injectable 0x13380 stand-in for the alloc-path lazy gate. Null = no auto-install (legacy).
    void set_lazy_bootstrap(const LazyBootstrapHooks& hooks);

    // 0x12820
    void* alloc(std::size_t size, std::uint32_t heap_id, std::size_t align, std::uint32_t flags);
    // 0x12af0
    void free(void* ptr, std::uint32_t kind);

    // 0x17070 / 0x17090 / 0x170b0 / 0x170c0
    void* new_(std::size_t size) { return alloc(size, kDefaultHeap, kDefaultAlign, kAllocFlagNew); }
    void* new_array(std::size_t size) {
        return alloc(size, kDefaultHeap, kDefaultAlign, kAllocFlagNewArray);
    }
    void delete_(void* ptr) { free(ptr, kAllocFlagNew); }
    void delete_array(void* ptr) { free(ptr, kAllocFlagNewArray); }

private:
    void report_oom(const OomInfo& info);
    // OBS 0x1284a..0x1286f. Returns true when bootstrap ran and heap_id should become slot 1.
    bool maybe_lazy_bootstrap(std::uint32_t& heap_id);

    std::array<std::atomic<IAllocator*>, kHeapSlots> table_{};
    PageOwnerMap owners_;
    std::atomic<std::uint32_t> tag_{0};
    std::atomic<std::uint32_t> oom_reports_{0};
    std::atomic<bool> oom_latch_{false};
    std::atomic<bool> bootstrap_latch_{false};  // OBS 0x2dfaaa0 (false == byte 0)
    std::mutex hooks_mu_;
    OomHooks hooks_{};
    LazyBootstrapHooks lazy_bootstrap_{};
};

// Per-thread scoped default-heap override (OBS stack of <= 32 heap ids, honoured only for heap 1).
class ScopedHeap {
public:
    explicit ScopedHeap(std::uint32_t heap_id) noexcept;
    ~ScopedHeap();
    ScopedHeap(const ScopedHeap&) = delete;
    ScopedHeap& operator=(const ScopedHeap&) = delete;
    bool active() const noexcept { return pushed_; }
    // Top of the calling thread's stack, or 0 when empty.
    static std::uint32_t top() noexcept;

private:
    bool pushed_ = false;
};

// Process-wide front-end used by the game_* wrappers (mirrors the single global table).
AllocatorFront& global_allocator_front() noexcept;
inline void* game_new(std::size_t size) { return global_allocator_front().new_(size); }
inline void* game_new_array(std::size_t size) { return global_allocator_front().new_array(size); }
inline void game_delete(void* p) { global_allocator_front().delete_(p); }
inline void game_delete_array(void* p) { global_allocator_front().delete_array(p); }

}  // namespace sotc
