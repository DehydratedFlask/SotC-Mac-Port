#include "vtable_15e2028_slot38_e19f70.hpp"

#include <cstring>

namespace sotc {

Vtable15e2028Slot38_e19f70Result vtable_15e2028_slot38_e19f70(
    const Vtable15e2028Slot38_e19f70Cells& cells,
    const Vtable15e2028Slot38_e19f70Hooks& hooks) noexcept {
    Vtable15e2028Slot38_e19f70Result out{};

    if (cells.key == nullptr) {
        out.early_return = true;
        out.code = 0;
        out.ok = true;
        return out;
    }
    if (cells.count == nullptr) {
        return out;  // the count is read before the array
    }
    if (*cells.count == 0) {
        out.early_return = true;
        out.code = 0;
        out.ok = true;
        return out;
    }
    if (cells.elements == nullptr) {
        return out;  // a non-empty container needs the array
    }
    if (hooks.element_name == nullptr) {
        return out;  // slot 0x10 is called for every element
    }

    for (std::uint32_t index = 0; index < *cells.count; ++index) {
        void* element = cells.elements[index];
        ++out.elements_scanned;

        const char* name = hooks.element_name(element);
        if (name == nullptr) {
            continue;
        }
        if (std::strcmp(name, cells.key) != 0) {
            continue;
        }
        ++out.name_matches;

        if (!out.found) {
            // The first match only records the index: the body jumps to `e1a006` without a call.
            out.found = true;
            out.best_index = static_cast<std::int32_t>(index);
            continue;
        }

        // A second match reaches both slot-0x18 reads, in this order: current, then best.
        if (hooks.element_id == nullptr) {
            return out;  // a second name match reaches slot 0x18 twice
        }
        const std::int32_t current = hooks.element_id(element);
        ++out.id_calls;
        void* best_element = cells.elements[static_cast<std::uint32_t>(out.best_index)];
        const std::int32_t best_id = hooks.element_id(best_element);
        ++out.id_calls;
        out.best_id = best_id;
        if (current > best_id) {  // `cmpl %eax,%ebx; jbe next`
            out.best_index = static_cast<std::int32_t>(index);
            out.best_id = current;
        }
    }

    out.code = out.found ? 1 : 0;  // `andb $0x1,%r15b; movl %r15d,%eax`
    out.ok = true;
    return out;
}

}  // namespace sotc
