#include "vslot_40_533100_cp3shadowprecache_attach.hpp"

#include "singleton_f08_531d84.hpp"

namespace sotc {

Vslot40_533100Cp3shadowprecacheAttachResult vslot_40_533100_cp3shadowprecache_attach(
    const Vslot40_533100Cp3shadowprecacheAttachHooks& hooks) noexcept {
    Vslot40AttachCommonHooks common = hooks.common;
    common.slot_base = hooks.self;
    common.lock_priority = kResourceWrapperLockPriority9;
    common.lock_after_cleanup = false;  // 0x53403c 0x140d90 tail
    return vslot_40_533100_attach_common(kCp3shadowprecacheKeyLiteral, kVslot40Cp3shadowprecacheAttachSlotOff,
                                            common);
}

}  // namespace sotc
