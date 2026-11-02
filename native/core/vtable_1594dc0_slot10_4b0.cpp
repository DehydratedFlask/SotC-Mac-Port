#include "vtable_1594dc0_slot10_4b0.hpp"

namespace sotc {

Vtable1594dc0Slot10_4b0Result vtable_1594dc0_slot10_4b0(
    const Vtable1594dc0Slot10_4b0Cells& cells,
    const Vtable1594dc0Slot10_4b0Hooks& hooks) noexcept {
    Vtable1594dc0Slot10_4b0Result out{};
    if (cells.vtable_field == nullptr || cells.array_begin == nullptr || cells.array_end == nullptr) {
        return out;  // the vtable store and both array reads are unconditional
    }

    out.vtable_written =
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(kVtable1594dc0Slot10InstalledTable));
    *cells.vtable_field = out.vtable_written;

    // The retail loop compares the running pointer with the end pointer; walking the same range here
    // keeps the element count identical without needing a raw host pointer.
    const auto begin = reinterpret_cast<std::uintptr_t>(*cells.array_begin);
    const auto end = reinterpret_cast<std::uintptr_t>(*cells.array_end);
    if (end > begin) {
        const std::uint64_t span = static_cast<std::uint64_t>(end - begin);
        out.elements = static_cast<std::uint32_t>(span / kVtable1594dc0Slot10ElementStride);
    }
    if (hooks.release_element != nullptr) {
        for (std::uint32_t index = 0; index < out.elements; ++index) {
            hooks.release_element(reinterpret_cast<void*>(
                begin + static_cast<std::uintptr_t>(index) * kVtable1594dc0Slot10ElementStride +
                kVtable1594dc0Slot10ElementFieldA));
            hooks.release_element(reinterpret_cast<void*>(
                begin + static_cast<std::uintptr_t>(index) * kVtable1594dc0Slot10ElementStride +
                kVtable1594dc0Slot10ElementFieldB));
            out.releases += 2;
        }
    }

    if (*cells.array_begin != nullptr) {
        AllocDispatchContract contract{};
        if (hooks.alloc != nullptr) {
            contract = *hooks.alloc;
        }
        const AllocWrapperResult freed = free_170c0(const_cast<void*>(*cells.array_begin), contract);
        out.free_tag = freed.tag;
        out.array_freed = freed.ok;
        if (!freed.ok) {
            return out;  // the retail body would free before the tail call; a failure is visible
        }
    }

    if (hooks.post_call != nullptr) {
        hooks.post_call(cells.self);
    }
    out.post_called = true;
    out.ok = true;
    return out;
}

}  // namespace sotc
