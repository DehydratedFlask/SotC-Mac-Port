#include "ctor_slot_491_295f90.hpp"

namespace sotc {

bool init_slot491_295f90(Slot491Objects& objects, StartupAtexitLog& cleanup) noexcept {
    bool ok = true;
    for (void*& handle : objects.handles) {
        handle = nullptr;                                            // call 0xaaf040
        ok = cleanup.record(kSlot491HandleDtorRva, &handle) && ok;   // __cxa_atexit(0xaaf050, &handle)
    }
    objects.caption.data = reinterpret_cast<const std::uint8_t*>(kSlot491CaptionText);   // 0x2960d0
    objects.caption.size = static_cast<std::int64_t>(sizeof kSlot491CaptionText - 1);    // 0x2960d7: 0x10
    objects.caption.aux = 0;                                                             // 0x2960e2: 16 B zero
    objects.caption.ctl = nullptr;
    objects.identity_vptr = kSlot491IdentityVptr;                                        // 0x2960ee
    ok = cleanup.record(kSlot491CaptionDtorRva, &objects.caption) && ok;                 // 0x2960ff tail jmp
    return ok;
}

}  // namespace sotc
