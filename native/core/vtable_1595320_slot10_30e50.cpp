#include "vtable_1595320_slot10_30e50.hpp"

namespace sotc {

Vtable1595320Slot10_30e50Result vtable_1595320_slot10_30e50(
    void* self, const Vtable1595320Slot10_30e50Hooks& hooks) noexcept {
    Vtable1595320Slot10_30e50Result out{};
    if (hooks.tail_312a0 == nullptr) {
        return out;  // the whole body is the forward
    }
    out.code = hooks.tail_312a0(self);
    out.forwarded = true;
    out.ok = true;
    return out;
}

}  // namespace sotc
