#include "store_value_clear_flag_158340.hpp"

#include <cstring>

namespace sotc {

void store_value_clear_flag_158340(void* object, std::uint32_t value) noexcept {
    auto* bytes = static_cast<unsigned char*>(object);
    std::memcpy(bytes, &value, sizeof value);            // mov [rdi],esi  (little endian host)
    bytes[kStoreValueClearFlag158340FlagOffset] = 0;     // mov byte [rdi+4],0
}

}  // namespace sotc
