#include "vtable_1594ae8_slot50_13d60.hpp"

#include <cstring>

namespace sotc {

Vtable1594ae8Slot50_13d60Result vtable_1594ae8_slot50_13d60(
    const Vtable1594ae8Slot50_13d60Cells& cells) noexcept {
    Vtable1594ae8Slot50_13d60Result out{};
    if (cells.object == nullptr) {
        return out;  // cannot model the store without the object
    }
    void* const stored = cells.object + kVtable1594ae8Slot50_13d60BaseOffset;
    void* previous = nullptr;
    std::memcpy(&previous, cells.object + kVtable1594ae8Slot50_13d60FieldOffset, sizeof(previous));
    std::memcpy(cells.object + kVtable1594ae8Slot50_13d60FieldOffset, &stored, sizeof(stored));
    out.stored = stored;
    out.previous = previous;
    out.ok = true;
    return out;
}

}  // namespace sotc
