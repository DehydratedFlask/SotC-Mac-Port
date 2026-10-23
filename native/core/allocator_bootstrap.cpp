#include "allocator_bootstrap.hpp"

namespace sotc {

std::optional<AllocatorBootstrapResult> bootstrap_allocator_hierarchy(
    AllocatorFront& front, CompositeAllocator& composite, BucketAllocator& bucket,
    HeapAllocator& heap, PageAllocator& page) noexcept {
    // OBS early-out: composite already present → treat as installed (do not rewire).
    if (front.heap(kBootstrapCompositeHeapSlot) != nullptr) {
        AllocatorBootstrapResult r;
        r.already_installed = true;
        r.composite_slot = kBootstrapCompositeHeapSlot;
        // Best-effort: report current occupants of the empty-table default slots.
        r.bucket_slot = kBootstrapBucketHeapSlot;
        r.heap_slot = kBootstrapHeapHeapSlot;
        r.page_slot = kBootstrapPageHeapSlot;
        return r;
    }

    if (composite.child_count() != 0) return std::nullopt;
    if (!install_default_hierarchy(composite, &bucket, &heap, &page)) return std::nullopt;

    // OBS: composite forced into slot 1 (not discovered by scan).
    if (!front.set_heap(kBootstrapCompositeHeapSlot, &composite)) return std::nullopt;

    const auto bucket_slot = first_free_heap_slot(front, 1);
    if (!bucket_slot || !front.set_heap(*bucket_slot, &bucket)) return std::nullopt;

    const auto heap_slot = first_free_heap_slot(front, 1);
    if (!heap_slot || !front.set_heap(*heap_slot, &heap)) return std::nullopt;

    const auto page_slot = first_free_heap_slot(front, 1);
    if (!page_slot || !front.set_heap(*page_slot, &page)) return std::nullopt;

    AllocatorBootstrapResult r;
    r.already_installed = false;
    r.composite_slot = kBootstrapCompositeHeapSlot;
    r.bucket_slot = *bucket_slot;
    r.heap_slot = *heap_slot;
    r.page_slot = *page_slot;
    return r;
}

}  // namespace sotc
