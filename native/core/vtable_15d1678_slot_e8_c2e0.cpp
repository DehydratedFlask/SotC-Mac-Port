#include "vtable_15d1678_slot_e8_c2e0.hpp"

namespace sotc {

Vtable15d1678SlotE8_c2e0Result vtable_15d1678_slot_e8_c2e0(
    const Vtable15d1678SlotE8_c2e0Cells& cells) noexcept {
    Vtable15d1678SlotE8_c2e0Result out{};
    if (cells.source_field == nullptr || cells.copy_field == nullptr) {
        return out;  // the read and the store are unconditional in the retail body
    }
    out.copied = const_cast<void*>(*cells.source_field);
    *cells.copy_field = out.copied;
    out.ok = true;
    return out;
}

}  // namespace sotc
