#include "owner_simple_ctors.hpp"

#include <cstring>

namespace sotc {

void init_owner_a1cc0(void* object) noexcept {
    (void)object;  // 0xa1cc0 is a single `ret`
}

void init_owner_fafb0(std::byte* object, void* vptr) noexcept {
    // The two recovered fills cover +0x08..+0x2007 and +0x2028..+0x82027; zeroing the whole object
    // first also makes the uninitialised +0x2020..+0x2027 deterministic.
    std::memset(object, 0, kOwnerFafb0ObjectSize);
    *reinterpret_cast<void**>(object) = vptr;
    *reinterpret_cast<void**>(object + 0x2008) = object + 0x2028;
    *reinterpret_cast<void**>(object + 0x2010) = object + kOwnerFafb0ObjectSize;
    *reinterpret_cast<void**>(object + 0x2018) = object + kOwnerFafb0ObjectSize;
}

bool init_owner_26f820(std::byte* object, void* vptr,
                       const Owner26f820Hooks& hooks) noexcept {
    if (hooks.base_ctor == nullptr || hooks.sub_ctor == nullptr) {
        return false;
    }
    if (!hooks.base_ctor(object)) {
        return false;
    }
    *reinterpret_cast<void**>(object) = vptr;
    *reinterpret_cast<void**>(object + 0x18) = nullptr;
    return hooks.sub_ctor(object + kOwner26f820SubOffset);
}

}  // namespace sotc
