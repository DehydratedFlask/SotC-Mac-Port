#include "shared_str_release_f590.hpp"

#include <atomic>

namespace sotc {

void release_shared_str_f590(SharedStr& str,
                             SharedStrDeallocatorF590& deallocator) noexcept {
    auto* const initial_ctl = str.ctl;
    if (initial_ctl == nullptr) return;

    // Integral atomic fetch_sub has defined two's-complement wrap, including
    // INT64_MIN -> INT64_MAX. Test the previous bits, as the locked xadd does.
    const auto previous = std::atomic_ref<std::int64_t>(*initial_ctl)
                              .fetch_sub(1, std::memory_order_seq_cst);
    if (previous != 1) return;

    deallocator.deallocate(str.data, SharedStrDeallocationKindF590::data);
    // The original reloads this+0x18 after the first external call. Keeping
    // initial_ctl here would miss callbacks that clear or replace the field.
    if (auto* const reloaded_ctl = str.ctl; reloaded_ctl != nullptr) {
        deallocator.deallocate(reloaded_ctl, SharedStrDeallocationKindF590::control);
    }
}

}  // namespace sotc
