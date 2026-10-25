#include "ctor_slot_300_acc880.hpp"

namespace sotc {

bool init_slot300_acc880(Slot300Cleanup& cleanup) noexcept {
    // __cxa_atexit(0xacc5b0 /* bare ret */, nullptr, 0x1604218): one inert entry, no writes.
    ++cleanup.registered;
    return true;
}

}  // namespace sotc
