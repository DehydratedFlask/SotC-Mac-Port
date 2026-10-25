// SRV-01: composite heap dispatch (original CPUMemory / composite object, allocate vt+0xa0 =
// 0x47e30; child registration 0x47dd0; bootstrap 0x13380 wires three children).
//
// OBS (private contract + ELF bytes; see tools/lift/check_f47e30.py):
//   * Children live in an array at object+0x90, stride 0x10: {threshold u64 @ -8, heap* @ 0}.
//   * allocate(size): walk children in order; first entry with size < threshold gets the vcall;
//     on success, racy [obj+0xd0] += size. Alignment and AllocDesc pass through unchanged.
//   * Bootstrap thresholds: Bucket 0x400, HeapAllocator 0x100000, PageAllocator ~0 (uint64 max).
//   * Optional "fast child" at +0xb8/+0xc0/+0xc8 is zero-initialised at bootstrap; not modelled.
//   * No zero-fill in this layer.
// INF (native design):
//   * deallocate forwards to the child that produced the pointer (original composite free 0x47f30
//     resolves via the global page-owner table 0x19a50, then jumps to that child's deallocate).
//   * Child list capacity is fixed (native); original grows with count at +0x80.
//   * Bootstrap wiring 0x13380: see allocator_bootstrap.hpp (lazy front-end latch still open).
//   * BucketAllocator / HeapAllocator: see bucket_allocator.hpp and heap_allocator.hpp.
#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "iallocator.hpp"

namespace sotc {

// OBS sizes from bootstrap 0x13380 / child registration 0x13645..0x13678.
inline constexpr std::uint64_t kCompositeBucketThreshold = 0x400;       // size < 0x400 -> bucket
inline constexpr std::uint64_t kCompositeHeapThreshold = 0x100000;      // size < 1 MiB -> heap
inline constexpr std::uint64_t kCompositePageThreshold = ~std::uint64_t{0};  // else -> page
inline constexpr std::uint64_t kHeapAllocatorCapBytes = 0x9600000;      // 150 MiB (0x48910 arg)
inline constexpr std::uint32_t kHeapPageBytes = 0x100000;               // 1 MiB (0x488f0 → +0xd0)
inline constexpr std::uint32_t kHeapReserveAlign = 0x10000;             // OBS 0x48910 edx → 0x479a0
inline constexpr std::uint32_t kBucketClassStep = 0x10;                 // classes 0x10..0x400
inline constexpr std::uint32_t kBucketClassMin = 0x10;
inline constexpr std::uint32_t kBucketPageBytes = 0x10000;              // 64 KiB pool pages

struct CompositeChild {
    std::uint64_t threshold = 0;  // OBS: accept when request size < threshold
    IAllocator* heap = nullptr;
};

class CompositeAllocator final : public IAllocator {
public:
    static constexpr std::size_t kMaxChildren = 8;  // INF; bootstrap uses 3

    CompositeAllocator() = default;
    ~CompositeAllocator() override = default;
    CompositeAllocator(const CompositeAllocator&) = delete;
    CompositeAllocator& operator=(const CompositeAllocator&) = delete;

    // OBS 0x47dd0: append {threshold, heap}. Returns false when full or heap is null.
    bool add_child(std::uint64_t threshold, IAllocator* heap) noexcept;
    // Bootstrap rollback for a hierarchy that has not been published to callers.
    void clear_children() noexcept { children_.clear(); }

    std::size_t child_count() const noexcept { return children_.size(); }
    const CompositeChild* children() const noexcept {
        return children_.empty() ? nullptr : children_.data();
    }

    void* allocate(std::size_t size, std::size_t align, const AllocDesc& desc) override;
    void deallocate(void* ptr, const AllocDesc& desc) override;
    void on_owner_map_deallocation(void* ptr) noexcept override;

    // OBS [obj+0xd0] success counter (racy in original; atomic here).
    std::uint64_t bytes_dispatched() const noexcept { return bytes_dispatched_.load(); }
    std::size_t tracked_allocation_count() const;

private:
    std::vector<CompositeChild> children_;
    std::atomic<std::uint64_t> bytes_dispatched_{0};
    mutable std::mutex mu_;
    std::unordered_map<void*, IAllocator*> live_;  // INF: ptr -> child for deallocate
};

// Install the three bootstrap thresholds onto an empty composite (children must outlive it).
inline bool install_default_hierarchy(CompositeAllocator& composite, IAllocator* bucket,
                                      IAllocator* heap, IAllocator* page) noexcept {
    if (!bucket || !heap || !page) return false;
    if (!composite.add_child(kCompositeBucketThreshold, bucket)) return false;
    if (!composite.add_child(kCompositeHeapThreshold, heap)) return false;
    if (!composite.add_child(kCompositePageThreshold, page)) return false;
    return true;
}

}  // namespace sotc
