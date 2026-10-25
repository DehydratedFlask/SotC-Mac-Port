#include "owner_ctor_b111e0_slot48.hpp"

namespace sotc {

bool owner_singleton_ensure(OwnerSingletonCell& cell,
                            const OwnerSlot48Hooks& hooks) noexcept {
    if (hooks.allocate == nullptr || hooks.deallocate == nullptr || hooks.construct == nullptr) {
        return false;
    }
    void* const object = hooks.allocate(kOwnerSingletonObjectSize);
    if (object == nullptr) {
        return false;
    }
    if (!hooks.construct(object)) {
        hooks.deallocate(object);
        return false;
    }

    void* const previous = cell.object;
    if (previous == object) {
        // 0x256999 `cmp rbx,r14 / je`: an unchanged pointer is neither destroyed nor stored again.
        return true;
    }
    if (previous != nullptr) {
        if (hooks.destroy == nullptr) {
            // The original has no null branch here; a missing hook must stay visible.
            hooks.deallocate(object);
            return false;
        }
        hooks.destroy(previous);
        hooks.deallocate(previous);
    }
    cell.object = object;
    return true;
}

bool owner_singleton_activate(void* object, const OwnerSlot48Hooks& hooks) noexcept {
    if (object == nullptr || hooks.user_service_init == nullptr ||
        hooks.share_utility_call == nullptr) {
        return false;
    }
    hooks.user_service_init(kOwnerUserServicePriority);
    hooks.share_utility_call(kOwnerShareUtilityArg0, kOwnerShareUtilityArg1,
                             kOwnerShareUtilityArg2, kOwnerShareUtilityArg3);
    static_cast<std::uint8_t*>(object)[kOwnerSingletonFlagOffset] = 1;
    return true;
}

bool owner_slot48_run(OwnerSingletonCell& cell, const OwnerSlot48Hooks& hooks) noexcept {
    if (!owner_singleton_ensure(cell, hooks)) {
        return false;
    }
    return owner_singleton_activate(cell.object, hooks);
}

}  // namespace sotc
