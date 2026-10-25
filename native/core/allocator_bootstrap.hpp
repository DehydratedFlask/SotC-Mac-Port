// SRV-01: allocator bootstrap wiring (original 0x13380).
//
// OBS (private contract + tools/lift/check_f13380.py):
//   * Early-out when the composite object pointer (originally [0x2dfb4d0]) is already set —
//     native: front.heap(kCompositeHeapSlot) != nullptr.
//   * Composite is published in heap-table slot 1 (originally store to tbl[1] @ 0x2dfaab8)
//     after its children are ready, so concurrent default allocations cannot observe a partial
//     hierarchy. Slot 1 is reserved while Bucket / Heap / Page scan for NULL slots from index 2;
//     this is equivalent to scanning from 1 after the original composite@1 store and yields
//     slots 2 / 3 / 4 on an empty table.
//   * Children are registered on the composite via 0x47dd0 in order:
//     Bucket thr 0x400, Heap thr 0x100000, Page thr ~0 (see install_default_hierarchy).
//   * Bucket ctor args page 0x10000 / step 0x10 / max 0x400; Heap page-set 0x488f0 (1 MiB)
//     then reserve 0x48910 (150 MiB). Native constructs with matching constants / owner ids.
// INF (native design):
//   * Does NOT run original 0x18aa0 page-table build, 0x42b80 name/guard setup, composite
//     vt+0x18 / +0x50 / +0x40 side effects, or the post-wire game_new(0x65690) via 0x17070.
//   * Does NOT auto-invoke from AllocatorFront::alloc (lazy latch byte 0x2dfaaa0 still open).
//   * Caller owns the four allocator objects; bootstrap only wires slots + child list.
//   * Owner ids passed into Bucket/Heap/Page must match the slots this helper assigns
//     (empty-table defaults below). No 0x19660 / freelist tourism.
#pragma once

#include <cstdint>
#include <optional>

#include "allocator_front.hpp"
#include "bucket_allocator.hpp"
#include "composite_allocator.hpp"
#include "heap_allocator.hpp"
#include "page_allocator.hpp"

namespace sotc {

// OBS empty-table outcomes after composite is forced into slot 1.
inline constexpr std::uint32_t kBootstrapCompositeHeapSlot = 1;
inline constexpr std::uint32_t kBootstrapBucketHeapSlot = 2;
inline constexpr std::uint32_t kBootstrapHeapHeapSlot = 3;
inline constexpr std::uint32_t kBootstrapPageHeapSlot = 4;
inline constexpr std::uint32_t kBootstrapHeapSlotCount = 0x100;  // OBS scan bound

// OBS slot scan: first index in [start, kBootstrapHeapSlotCount) with a null heap entry.
inline std::optional<std::uint32_t> first_free_heap_slot(const AllocatorFront& front,
                                                        std::uint32_t start = 1) noexcept {
    if (start == 0 || start >= AllocatorFront::kHeapSlots) return std::nullopt;
    const std::uint32_t end = AllocatorFront::kHeapSlots < kBootstrapHeapSlotCount
                                  ? AllocatorFront::kHeapSlots
                                  : kBootstrapHeapSlotCount;
    for (std::uint32_t i = start; i < end; ++i) {
        if (front.heap(i) == nullptr) return i;
    }
    return std::nullopt;
}

struct AllocatorBootstrapResult {
    bool already_installed = false;
    std::uint32_t composite_slot = 0;
    std::uint32_t bucket_slot = 0;
    std::uint32_t heap_slot = 0;
    std::uint32_t page_slot = 0;
};

// Wire composite + three children into `front` and install bootstrap thresholds.
// Requires: bucket/heap/page constructed with owner ids matching the slots this will assign
// (use the kBootstrap*HeapSlot constants when starting from an empty front).
// Returns nullopt if hierarchy install or slot assignment fails.
std::optional<AllocatorBootstrapResult> bootstrap_allocator_hierarchy(
    AllocatorFront& front, CompositeAllocator& composite, BucketAllocator& bucket,
    HeapAllocator& heap, PageAllocator& page) noexcept;

}  // namespace sotc
