#include "vtable_1594dc0_slot20_20d60.hpp"

namespace sotc {

Vtable1594dc0Slot20_20d60Result vtable_1594dc0_slot20_20d60(
    void* self, const Vtable1594dc0Slot20_20d60Cells& cells,
    const Vtable1594dc0Slot20_20d60Hooks& hooks) noexcept {
    Vtable1594dc0Slot20_20d60Result out{};
    if (cells.once_flag == nullptr) {
        return out;  // cannot model the gate without the flag byte
    }

    if (*cells.once_flag != 0) {
        out.early_return = true;
        out.ok = true;
        return out;
    }

    *cells.once_flag = 1;
    out.flag_written = true;

    if (cells.global_cell != nullptr) {
        out.global_value = const_cast<void*>(*cells.global_cell);
    }
    if (hooks.tail_call != nullptr) {
        hooks.tail_call(self, out.global_value);
        out.tail_call_modelled = true;
    }
    out.ok = true;
    return out;
}

}  // namespace sotc
