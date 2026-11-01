#include "vtable_15e2028_slot70_e1a5c0.hpp"

#include <cstring>

namespace sotc {

Vtable15e2028Slot70_e1a5c0Result vtable_15e2028_slot70_e1a5c0(
    const Vtable15e2028Slot70_e1a5c0Cells& cells,
    const Vtable15e2028Slot70_e1a5c0Hooks& hooks) noexcept {
    Vtable15e2028Slot70_e1a5c0Result out{};

    if (cells.key == nullptr) {
        out.early_return = true;
        out.code = 1;
        out.ok = true;
        return out;
    }
    if (cells.count == nullptr) {
        return out;  // the count is read before the array
    }
    if (*cells.count == 0) {
        out.early_return = true;
        out.code = 1;
        out.ok = true;
        return out;
    }
    if (cells.elements == nullptr) {
        return out;  // a non-empty container needs the array
    }
    if (hooks.element_key == nullptr || hooks.element_value == nullptr) {
        return out;  // both element slots are called for every scanned element
    }

    for (std::uint32_t index = 0; index < *cells.count; ++index) {
        void* element = cells.elements[index];
        ++out.elements_scanned;

        const char* element_key = hooks.element_key(element);
        if (element_key == nullptr) {
            continue;  // `testq %rax,%rax; je next`
        }
        if (std::strcmp(element_key, cells.key) != 0) {
            continue;  // `call strcmp; testl %eax,%eax; jne next`
        }
        if (hooks.element_value(element) != cells.value) {
            continue;  // `cmpl %r14d,%eax; je found`
        }

        out.found_index = static_cast<std::int32_t>(index);
        if (hooks.element_match != nullptr) {
            out.code = hooks.element_match(element);
        }
        out.match_called = true;
        out.ok = true;
        return out;
    }

    out.code = 1;  // `movl $0x1,%eax`
    out.ok = true;
    return out;
}

}  // namespace sotc
