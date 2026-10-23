#include "vslot_40_533100_shadow_attach.hpp"

#include "singleton_f08_531d84.hpp"

namespace sotc {

Vslot40_533100ShadowAttachResult vslot_40_533100_shadow_attach(
    const Vslot40_533100ShadowAttachHooks& hooks) noexcept {
    Vslot40AttachCommonHooks common = hooks.common;
    if (common.slot_base == nullptr) common.slot_base = singleton_f08_cell_2efc9e0();
    return vslot_40_533100_attach_common(kShadowKeyLiteral, kVslot40ShadowAttachSlotOff,
                                            common);
}

}  // namespace sotc
