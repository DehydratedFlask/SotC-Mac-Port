#include "allocator_bootstrap.hpp"

#include <mutex>

namespace sotc {
namespace {

std::mutex g_bootstrap_mutex;

std::optional<std::uint32_t> find_heap_slot(const AllocatorFront& front,
                                            const IAllocator* heap) noexcept {
    if (heap == nullptr) return std::nullopt;
    for (std::uint32_t slot = 2; slot < kBootstrapHeapSlotCount; ++slot) {
        if (front.heap(slot) == heap) return slot;
    }
    return std::nullopt;
}

bool has_default_children(const CompositeAllocator& composite, BucketAllocator& bucket,
                          HeapAllocator& heap, PageAllocator& page) noexcept {
    if (composite.child_count() != 3) return false;
    const CompositeChild* children = composite.children();
    return children != nullptr &&
           children[0].threshold == kCompositeBucketThreshold && children[0].heap == &bucket &&
           children[1].threshold == kCompositeHeapThreshold && children[1].heap == &heap &&
           children[2].threshold == kCompositePageThreshold && children[2].heap == &page;
}

}  // namespace

std::optional<AllocatorBootstrapResult> bootstrap_allocator_hierarchy(
    AllocatorFront& front, CompositeAllocator& composite, BucketAllocator& bucket,
    HeapAllocator& heap, PageAllocator& page) noexcept {
    // The lazy front-end hook may be entered concurrently by multiple first allocations.
    // Serialize construction so the composite child vector and table slots have one writer.
    std::lock_guard<std::mutex> bootstrap_lock(g_bootstrap_mutex);

    IAllocator* installed = front.heap(kBootstrapCompositeHeapSlot);
    if (installed != nullptr) {
        // A non-null slot is not enough: only this fully wired hierarchy is idempotent.
        if (installed != &composite ||
            !has_default_children(composite, bucket, heap, page)) {
            return std::nullopt;
        }
        const auto bucket_slot = find_heap_slot(front, &bucket);
        const auto heap_slot = find_heap_slot(front, &heap);
        const auto page_slot = find_heap_slot(front, &page);
        if (!bucket_slot || !heap_slot || !page_slot) return std::nullopt;

        AllocatorBootstrapResult r;
        r.already_installed = true;
        r.composite_slot = kBootstrapCompositeHeapSlot;
        r.bucket_slot = *bucket_slot;
        r.heap_slot = *heap_slot;
        r.page_slot = *page_slot;
        return r;
    }

    if (composite.child_count() != 0) return std::nullopt;
    std::optional<std::uint32_t> bucket_slot;
    std::optional<std::uint32_t> heap_slot;
    std::optional<std::uint32_t> page_slot;
    auto rollback = [&]() noexcept {
        if (page_slot) front.clear_heap_if(*page_slot, &page);
        if (heap_slot) front.clear_heap_if(*heap_slot, &heap);
        if (bucket_slot) front.clear_heap_if(*bucket_slot, &bucket);
        composite.clear_children();
    };

    if (!install_default_hierarchy(composite, &bucket, &heap, &page)) {
        rollback();
        return std::nullopt;
    }

    // Keep slot 1 unpublished until the complete child set is visible. Reserving it during
    // child scans is equivalent to the observed sequence that wrote the composite there first.
    auto candidate = first_free_heap_slot(front, 2);
    if (!candidate || !front.set_heap_if_empty(*candidate, &bucket)) {
        rollback();
        return std::nullopt;
    }
    bucket_slot = candidate;

    candidate = first_free_heap_slot(front, 2);
    if (!candidate || !front.set_heap_if_empty(*candidate, &heap)) {
        rollback();
        return std::nullopt;
    }
    heap_slot = candidate;

    candidate = first_free_heap_slot(front, 2);
    if (!candidate || !front.set_heap_if_empty(*candidate, &page)) {
        rollback();
        return std::nullopt;
    }
    page_slot = candidate;

    // This atomic publication is the commit point: allocators cannot observe a partial child
    // vector through the default slot, and a competing slot-1 owner is never overwritten.
    if (!front.set_heap_if_empty(kBootstrapCompositeHeapSlot, &composite)) {
        rollback();
        return std::nullopt;
    }

    AllocatorBootstrapResult r;
    r.already_installed = false;
    r.composite_slot = kBootstrapCompositeHeapSlot;
    r.bucket_slot = *bucket_slot;
    r.heap_slot = *heap_slot;
    r.page_slot = *page_slot;
    return r;
}

}  // namespace sotc
