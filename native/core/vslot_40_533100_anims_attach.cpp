#include "vslot_40_533100_anims_attach.hpp"

#include "singleton_f08_531d84.hpp"

namespace sotc {

Vslot40_533100AnimsAttachResult vslot_40_533100_anims_attach(
    const Vslot40_533100AnimsAttachHooks& hooks) noexcept {
    Vslot40AttachCommonHooks common = hooks.common;
    if (common.slot_base == nullptr) common.slot_base = singleton_f08_cell_2efc9e0();
    return vslot_40_533100_attach_common(kAnimsKeyLiteral, kVslot40AnimsAttachSlotOff,
                                            common);
}

}  // namespace sotc
