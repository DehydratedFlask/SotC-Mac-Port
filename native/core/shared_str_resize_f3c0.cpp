#include "shared_str_resize_f3c0.hpp"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <limits>

namespace sotc {
namespace {

// The four original paths plus the native failure result. The public bool only maps path 4 to
// true; the enum keeps the distinction so `terminate_shared_str_f6b0` can tell a successful
// path-1/path-2 resize (original returns 0) from a rejected one.
enum class ResizeOutcome {
    no_change,                   // 0xf50a: capacity sufficient, only size written
    allocated_control_null,      // 0xf40e: control was null, data + control allocated
    allocated_shared,            // 0xf470: shared control, data + new control allocated
    reallocated_capacity_short,  // 0xf513: unique control, capacity short; original al == 1
    failed,                      // native: rejected input or failed allocation
};

[[nodiscard]] bool allocation_hooks_ready(const SharedStrCtorHooks& hooks) noexcept {
    return hooks.alloc.allocate != nullptr && hooks.alloc.deallocate != nullptr;
}

[[nodiscard]] bool holder_is_sane(const SharedStr& str) noexcept {
    return str.size >= 0 && (str.size == 0 || str.data != nullptr);
}

void copy_prefix(void* dst, const void* src, std::size_t length) noexcept {
    if (length != 0) {
        std::memcpy(dst, src, length);
    }
}

void free_owned(const SharedStrCtorHooks& hooks, const void* pointer,
                std::uint32_t tag) noexcept {
    if (pointer != nullptr) {
        (void)hooks.alloc.deallocate(const_cast<void*>(pointer), tag);
    }
}

[[nodiscard]] std::uint64_t align_down(std::uint64_t value,
                                       std::uint64_t alignment) noexcept {
    return value & ~(alignment - 1);
}

// 0xf40e. `control == 0`: allocate data and a fresh control block. The original does not free the
// old data on this path; the normal old value is the shared empty static, not a heap allocation.
[[nodiscard]] ResizeOutcome resize_control_null(SharedStr& str, std::int64_t new_len,
                                                const SharedStrCtorHooks& hooks) noexcept {
    const std::uint64_t len = static_cast<std::uint64_t>(new_len);
    const std::uint64_t alloc = align_down(len + 4, kSharedStrResizeAlign4);

    void* const new_data = hooks.alloc.allocate(alloc, kSharedStrResizeDataTag, 8, 0);
    if (new_data == nullptr) {
        return ResizeOutcome::failed;
    }
    void* const new_control =
        hooks.alloc.allocate(8, kSharedStrResizeControlTag, 8, 1);
    if (new_control == nullptr) {
        free_owned(hooks, new_data, kSharedStrResizeDataTag);
        return ResizeOutcome::failed;
    }

    const auto copy_length = static_cast<std::size_t>(
        std::min<std::int64_t>(str.size, new_len));
    copy_prefix(new_data, str.data, copy_length);

    std::atomic_ref<std::int64_t>(*static_cast<std::int64_t*>(new_control))
        .store(1, std::memory_order_seq_cst);

    str.data = static_cast<const std::uint8_t*>(new_data);
    str.size = new_len;
    str.aux = static_cast<std::int64_t>(alloc - 1);
    str.ctl = static_cast<std::int64_t*>(new_control);
    return ResizeOutcome::allocated_control_null;
}

// 0xf470. Shared control (`*control != 1`): allocate replacement data and control, then release
// the old reference. The replacement control is allocated before the old reference is released
// so a failed control allocation cannot leave the holder pointing at a freed counter; the
// original allocates it after the release and would crash on failure.
[[nodiscard]] ResizeOutcome resize_shared(SharedStr& str, std::int64_t new_len,
                                          const SharedStrCtorHooks& hooks) noexcept {
    const std::uint64_t len = static_cast<std::uint64_t>(new_len);
    const std::uint64_t alloc = align_down(len + 4, kSharedStrResizeAlign4);

    void* const new_data = hooks.alloc.allocate(alloc, kSharedStrResizeDataTag, 8, 0);
    if (new_data == nullptr) {
        return ResizeOutcome::failed;
    }
    void* const new_control =
        hooks.alloc.allocate(8, kSharedStrResizeControlTag, 8, 1);
    if (new_control == nullptr) {
        free_owned(hooks, new_data, kSharedStrResizeDataTag);
        return ResizeOutcome::failed;
    }

    const auto copy_length = static_cast<std::size_t>(
        std::min<std::int64_t>(str.size, new_len));
    copy_prefix(new_data, str.data, copy_length);

    // 0xf4b0 reloads [holder+0x18], then `lock xadd -1`; the old data/control are freed only when
    // the previous count was 1.
    std::int64_t* const old_control = str.ctl;
    const std::int64_t previous = std::atomic_ref<std::int64_t>(*old_control)
                                      .fetch_sub(1, std::memory_order_seq_cst);
    if (previous == 1) {
        free_owned(hooks, str.data, kSharedStrResizeDataTag);
        // 0xf4d5 reloads the holder field before the tag-1 free.
        if (std::int64_t* const reloaded = str.ctl; reloaded != nullptr) {
            free_owned(hooks, reloaded, kSharedStrResizeControlTag);
        }
    }

    std::atomic_ref<std::int64_t>(*static_cast<std::int64_t*>(new_control))
        .store(1, std::memory_order_seq_cst);

    str.data = static_cast<const std::uint8_t*>(new_data);
    str.size = new_len;
    str.aux = static_cast<std::int64_t>(alloc - 1);
    str.ctl = static_cast<std::int64_t*>(new_control);
    return ResizeOutcome::allocated_shared;
}

// 0xf513. Unique control, capacity short: data reallocated with one of the three size-class
// alignments, old `size` bytes copied, old data freed with tag 0, control block kept.
[[nodiscard]] ResizeOutcome resize_unique_short_capacity(
    SharedStr& str, std::int64_t new_len, const SharedStrCtorHooks& hooks) noexcept {
    const std::uint64_t len = static_cast<std::uint64_t>(new_len);
    std::uint64_t alloc = 0;
    if (new_len <= kSharedStrResizeSmallLimit) {
        alloc = align_down(len + kSharedStrResizeSmallSlack,
                           kSharedStrResizeSmallAlign);
    } else if (new_len <= kSharedStrResizeMediumLimit) {
        alloc = align_down(len + kSharedStrResizeMediumSlack,
                           kSharedStrResizeMediumAlign);
    } else {
        alloc = align_down(len + kSharedStrResizeLargeSlack,
                           kSharedStrResizeLargeAlign);
    }

    // The original copies the old `size` bytes. A holder obeying capacity >= size cannot reach
    // this check with a larger size; reject it rather than overrun the replacement buffer.
    if (static_cast<std::uint64_t>(str.size) > alloc) {
        return ResizeOutcome::failed;
    }

    void* const new_data = hooks.alloc.allocate(alloc, kSharedStrResizeDataTag, 8, 0);
    if (new_data == nullptr) {
        return ResizeOutcome::failed;
    }

    copy_prefix(new_data, str.data, static_cast<std::size_t>(str.size));
    free_owned(hooks, str.data, kSharedStrResizeDataTag);

    str.data = static_cast<const std::uint8_t*>(new_data);
    str.aux = static_cast<std::int64_t>(alloc - 1);
    str.size = new_len;
    return ResizeOutcome::reallocated_capacity_short;
}

[[nodiscard]] ResizeOutcome resize_impl(SharedStr& str, std::int64_t new_len,
                                        const SharedStrCtorHooks& hooks) noexcept {
    if (new_len < 0 || !holder_is_sane(str)) {
        return ResizeOutcome::failed;
    }
    // The original adds at most 0x400 before aligning; reject a length that would wrap int64.
    if (new_len > std::numeric_limits<std::int64_t>::max() - 0x400) {
        return ResizeOutcome::failed;
    }

    if (str.ctl == nullptr) {
        if (!allocation_hooks_ready(hooks)) {
            return ResizeOutcome::failed;
        }
        return resize_control_null(str, new_len, hooks);
    }

    const std::int64_t count =
        std::atomic_ref<std::int64_t>(*str.ctl).load(std::memory_order_relaxed);
    if (count != 1) {
        if (!allocation_hooks_ready(hooks)) {
            return ResizeOutcome::failed;
        }
        return resize_shared(str, new_len, hooks);
    }

    // 0xf3ea/0xf3ee compare the capacity with the requested length using `jae` (unsigned).
    if (static_cast<std::uint64_t>(str.aux) >=
        static_cast<std::uint64_t>(new_len)) {
        str.size = new_len;
        return ResizeOutcome::no_change;
    }

    if (!allocation_hooks_ready(hooks)) {
        return ResizeOutcome::failed;
    }
    return resize_unique_short_capacity(str, new_len, hooks);
}

}  // namespace

bool resize_shared_str_f3c0(SharedStr& str, std::int64_t new_len,
                            const SharedStrCtorHooks& hooks) noexcept {
    return resize_impl(str, new_len, hooks) ==
           ResizeOutcome::reallocated_capacity_short;
}

bool terminate_shared_str_f6b0(SharedStr& str, std::int64_t new_len,
                               const SharedStrCtorHooks& hooks) noexcept {
    if (resize_impl(str, new_len, hooks) == ResizeOutcome::failed) {
        return false;
    }
    // The original writes data[size] without a check. Keep the write inside the recovered
    // allocation invariant (allocation size == capacity + 1, capacity >= size).
    if (str.data == nullptr || str.size < 0 || str.size > str.aux) {
        return false;
    }
    const_cast<std::uint8_t*>(str.data)[str.size] = 0;
    return true;
}

}  // namespace sotc
