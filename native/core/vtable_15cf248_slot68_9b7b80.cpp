#include "vtable_15cf248_slot68_9b7b80.hpp"

#include <cstring>

namespace sotc {

Vtable15cf248Slot68_9b7b80Result vtable_15cf248_slot68_9b7b80(
    const Vtable15cf248Slot68_9b7b80Cells& cells) noexcept {
    Vtable15cf248Slot68_9b7b80Result out{};
    if (cells.self == nullptr) {
        return out;  // `vmovss 0x38(%rdi)` dereferences the cell unconditionally
    }
    std::memcpy(&out.value, cells.self + kVtable15cf248Slot68_9b7b80ValueOffset,
                sizeof(out.value));
    out.ok = true;
    return out;
}

}  // namespace sotc
