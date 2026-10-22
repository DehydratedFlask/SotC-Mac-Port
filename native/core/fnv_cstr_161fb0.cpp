#include "fnv_cstr_161fb0.hpp"

namespace sotc {

std::uint32_t fnv_cstr_161fb0(const char* s, std::uint32_t seed) noexcept {
    if (s == nullptr) return 0;   // original returns 0 for a null pointer whatever the seed is
    std::uint32_t h = seed;
    for (; *s != '\0'; ++s) {
        // movsx ecx, cl : sign-extend the byte to 32 bits, then xor, then imul by the FNV prime
        const auto c = static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(*s)));
        h = (h ^ c) * 0x01000193u;
    }
    return h;
}

std::uint32_t fnv_cstr_52a990(const char* s) noexcept { return fnv_cstr_161fb0(s, kFnvOffsetBasis52a990); }

}  // namespace sotc
