#include "vtable_1594ae8_slot28_13d20.hpp"

namespace sotc {

Vtable1594ae8Slot28_13d20Result vtable_1594ae8_slot28_13d20(
    std::uint32_t value, const Vtable1594ae8Slot28_13d20Cells& cells) noexcept {
    Vtable1594ae8Slot28_13d20Result out{};
    if (cells.field == nullptr) {
        return out;  // cannot model the store without the field
    }
    out.previous = *cells.field;
    *cells.field = value;
    out.written = value;
    out.ok = true;
    return out;
}

}  // namespace sotc
