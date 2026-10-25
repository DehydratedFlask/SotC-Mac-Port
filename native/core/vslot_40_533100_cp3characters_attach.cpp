#include "vslot_40_533100_cp3characters_attach.hpp"

#include "singleton_f08_531d84.hpp"

namespace sotc {

Vslot40_533100Cp3charactersAttachResult vslot_40_533100_cp3characters_attach(
    const Vslot40_533100Cp3charactersAttachHooks& hooks) noexcept {
    Vslot40AttachCommonHooks common = hooks.common;
    common.slot_base = hooks.self;
    return vslot_40_533100_attach_common(kCp3charactersKeyLiteral, kVslot40Cp3charactersAttachSlotOff,
                                            common);
}

}  // namespace sotc
