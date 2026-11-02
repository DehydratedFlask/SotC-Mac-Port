#include "vtable_15ed2a0_slot10_e52cb0.hpp"

#include <cstring>

namespace sotc {

Vtable15ed2a0Slot10_e52cb0Result vtable_15ed2a0_slot10_e52cb0(
    const Vtable15ed2a0Slot10_e52cb0Cells& cells,
    const Vtable15ed2a0Slot10_e52cb0Hooks& hooks) noexcept {
    Vtable15ed2a0Slot10_e52cb0Result out{};

    if (cells.self == nullptr || cells.vtable_field == nullptr) {
        return out;  // the vtable stamp is the first store
    }
    const std::uint64_t installed = kVtable15ed2a0Slot10_e52cb0InstalledTable;
    std::memcpy(cells.vtable_field, &installed, sizeof(installed));
    out.vtable_written = true;

    // Entry loop: pointer at record+0, flag byte at record+8, stride 0x10.
    if (cells.entry_count == nullptr) {
        return out;  // `movl 0x70(%r14),%eax` is unconditional
    }
    if (*cells.entry_count > 0) {
        if (cells.entry_base_field == nullptr || cells.manager_field == nullptr ||
            hooks.free_entry == nullptr) {
            return out;  // every record on this path calls manager slot 0xd0
        }
        for (std::int32_t index = 0; index < *cells.entry_count; ++index) {
            const std::uint8_t* base = *cells.entry_base_field;  // reloaded each iteration
            if (base == nullptr) {
                return out;
            }
            const std::uint64_t record =
                static_cast<std::uint64_t>(index) * kVtable15ed2a0Slot10_e52cb0EntryStride;
            std::uint8_t flag = 0;
            std::memcpy(&flag, base + record + kVtable15ed2a0Slot10_e52cb0EntryFlagOffset,
                        sizeof(flag));
            if (flag == 0) {
                continue;  // `cmpb $0x0,...; je next`
            }
            void* entry = nullptr;
            std::memcpy(&entry, base + record, sizeof(entry));
            if (entry == nullptr) {
                continue;
            }
            hooks.free_entry(*cells.manager_field, entry);
            ++out.entries_freed;
        }
    }

    // Element loop: destroy each non-null element through its vtable slot 0x10.
    if (cells.element_count == nullptr) {
        return out;  // `movl 0x58(%r14),%eax` is unconditional
    }
    if (*cells.element_count > 0) {
        if (cells.elements_field == nullptr || hooks.destroy_element == nullptr) {
            return out;
        }
        for (std::int32_t index = 0; index < *cells.element_count; ++index) {
            void* const* array = *cells.elements_field;  // reloaded each iteration
            if (array == nullptr) {
                return out;
            }
            void* element = array[index];
            if (element == nullptr) {
                continue;
            }
            hooks.destroy_element(element);
            ++out.elements_destroyed;
        }
    }

    // Three conditional buffer releases, in this order: elements, entries, third.
    if (cells.elements_owned == nullptr || cells.entries_owned == nullptr ||
        cells.third_owned == nullptr) {
        return out;  // the three flag bytes are read unconditionally
    }
    const auto release = [&](bool owned, const void* buffer) -> bool {
        if (!owned || buffer == nullptr) {
            return true;  // flag 0 or a null buffer skips the call
        }
        if (cells.manager_field == nullptr || hooks.release_buffer == nullptr) {
            return false;
        }
        hooks.release_buffer(*cells.manager_field, buffer);
        ++out.buffers_released;
        return true;
    };
    const void* elements = cells.elements_field == nullptr ? nullptr : *cells.elements_field;
    const void* entries = cells.entry_base_field == nullptr ? nullptr : *cells.entry_base_field;
    const void* third = cells.third_field == nullptr ? nullptr : *cells.third_field;
    if (!release(*cells.elements_owned != 0, elements)) {
        return out;
    }
    if (!release(*cells.entries_owned != 0, entries)) {
        return out;
    }
    if (!release(*cells.third_owned != 0, third)) {
        return out;
    }

    if (hooks.base_dtor_e1ede0 == nullptr) {
        return out;  // the body ends in the base-destructor tail jump
    }
    hooks.base_dtor_e1ede0(cells.self);
    out.base_dtor_called = true;
    out.ok = true;
    return out;
}

}  // namespace sotc
