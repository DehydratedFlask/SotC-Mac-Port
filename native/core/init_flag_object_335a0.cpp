#include "init_flag_object_335a0.hpp"

#include <cstring>

namespace sotc {

void init_flag_object_335a0(void* obj) noexcept {
    auto* p = static_cast<std::uint8_t*>(obj);
    // OBS writes vtable AP at +0; native keeps null so no guest vcall dispatch.
    std::memset(p, 0, kFlagObject335a0Bytes);
    // +0x10/+0x20/+0x30 cleared (matches mov/xchg zeros).
}

}  // namespace sotc
