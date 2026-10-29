#include "ctor_slot_364_74a8e0.hpp"

#include <cstring>

namespace sotc {

Slot364Result init_slot364_74a8e0(Slot364Objects& objects, ConCmdRegistry& concmd,
                                  DebugMenuRegistry& menu) noexcept {
    Slot364Result out{};
    out.var = init_float_var_static(objects.var, kSlot364Spec, concmd);
    if (!out.var.ok) return out;
    float lo = 0.0f;
    float hi = 0.0f;
    std::memcpy(&lo, &kSlot364RangeMinBits, 4);
    std::memcpy(&hi, &kSlot364RangeMaxBits, 4);
    out.item = lane_m1::init_item_27050_site(objects.item, &objects.var, kSlot364ItemLabel, kSlot364ItemText, kSlot364ItemFlags, lo,
                                             hi, concmd, menu);
    out.ok = out.var.ok && out.item.ok;
    return out;
}

}  // namespace sotc
