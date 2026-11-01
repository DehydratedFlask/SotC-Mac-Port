#include "vtable_15f2f08_slot38_e16a00.hpp"

namespace sotc {

Vtable15f2f08Slot38_e16a00Result vtable_15f2f08_slot38_e16a00(
    const Vtable15f2f08Slot38_e16a00Cells& cells,
    const Vtable15f2f08Slot38_e16a00Hooks& hooks) noexcept {
    Vtable15f2f08Slot38_e16a00Result out{};
    if (cells.self == nullptr || cells.vtable_field == nullptr) {
        return out;  // `mov (%rbx),%rax` would fault
    }

    out.vtable = *cells.vtable_field;
    if (out.vtable == nullptr) {
        return out;  // null vtable: both calls would fault
    }

    if (hooks.call_slot18 != nullptr) {
        hooks.call_slot18(cells.self, cells.argument);
    }
    out.first_call_modelled = true;

    // The retail body reloads the vtable before the tail call; read the cell again.
    out.vtable = *cells.vtable_field;
    if (out.vtable == nullptr) {
        return out;
    }

    if (hooks.tail_slot20 != nullptr) {
        hooks.tail_slot20(cells.self, cells.argument);
    }
    out.tail_modelled = true;
    out.ok = true;
    return out;
}

}  // namespace sotc
