#include "owner_ctor_b11210_slot58.hpp"

namespace sotc {

OwnerSlot58Result owner_slot58_run(const OwnerSlot58Hooks& hooks) noexcept {
    OwnerSlot58Result out{};
    if (hooks.retail_build == nullptr) {
        return out;
    }
    if (hooks.retail_build()) {
        out.ok = true;
        out.gated = true;
        return out;
    }
    if (hooks.bag_singleton == nullptr || hooks.insert_tree0 == nullptr) {
        return out;
    }
    void* const bag = hooks.bag_singleton();
    if (bag == nullptr) {
        // The original would dereference a null bag; the native model reports it instead.
        return out;
    }
    out.inserted = hooks.insert_tree0(bag, kOwnerSlot58Key, kOwnerSlot58ValueInt,
                                     kOwnerSlot58ValueFloat, kOwnerSlot58Tag);
    out.ok = out.inserted;
    return out;
}

}  // namespace sotc
