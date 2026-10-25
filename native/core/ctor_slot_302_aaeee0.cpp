#include "ctor_slot_302_aaeee0.hpp"

namespace sotc {

bool init_slot302_aaeee0(Slot302Objects& objects, StartupAtexitLog& cleanup) noexcept {
    bool ok = true;
    for (void*& handle : objects.handles) {
        handle = nullptr;  // 0xaaf040: one qword store, no release of an earlier value
        const bool recorded = cleanup.record(kSlot302DtorRva, &handle);
        ok = recorded && ok;
    }
    objects.identity_vptr = kSlot302IdentityVptr;
    return ok;
}

}  // namespace sotc
