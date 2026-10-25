#include "owner_dtor_1bdae0.hpp"

namespace sotc {

OwnerDtor1bdae0Result owner_dtor_1bdae0(void** slot,
                                        const OwnerDtor1bdae0Hooks& hooks) noexcept {
    OwnerDtor1bdae0Result out{};
    if (slot == nullptr) {
        return out;  // no slot: visible failure
    }
    void* owned = *slot;
    out.owned = owned;
    if (owned == nullptr) {
        out.was_empty = true;
        out.ok = true;  // the original's `je` path does nothing and returns
        return out;
    }

    // call 0x1bede0(owned)
    if (hooks.destroy == nullptr) {
        return out;
    }
    hooks.destroy(owned);
    out.destroyed = true;

    // call 0x170b0(owned)
    if (hooks.deallocate == nullptr) {
        return out;
    }
    hooks.deallocate(owned);
    out.deallocated = true;

    // [r14] = 0
    *slot = nullptr;
    out.slot_cleared = true;
    out.ok = true;
    return out;
}

}  // namespace sotc
