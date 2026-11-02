#include "vtable_15e2028_slot40_e1a040.hpp"

#include <cstring>

namespace sotc {

Vtable15e2028Slot40_e1a040Result vtable_15e2028_slot40_e1a040(
    const Vtable15e2028Slot40_e1a040Cells& cells,
    const Vtable15e2028Slot40_e1a040Hooks& hooks) noexcept {
    Vtable15e2028Slot40_e1a040Result out{};

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
            continue;  // `testq %rax,%rax; je next`
        }
        if (std::strcmp(name, cells.key) != 0) {
            continue;  // `call strcmp; testl %eax,%eax; jne next`
        }
        ++out.names_compared;
        if (hooks.element_id == nullptr) {
            return out;  // a name match reaches slot 0x18
        }
        if (hooks.element_id(element) == cells.value) {
            out.found_index = static_cast<std::int32_t>(index);
            out.code = 1;  // `movb $0x1,%al` before the compare
            out.ok = true;
            return out;
        }
    }

    out.code = 0;  // `xorl %eax,%eax`
    out.ok = true;
    return out;
}

}  // namespace sotc
