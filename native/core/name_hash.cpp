#include "name_hash.hpp"

namespace sotc {

namespace {
constexpr std::uint32_t kSeed = 5381u;
}  // namespace

std::uint32_t name_hash(const char* name) noexcept {
    if (name == nullptr) {
        return 0;
    }
    std::uint32_t hash = kSeed;
    for (const char* p = name; *p != '\0'; ++p) {
        const auto byte = static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(*p)));
        hash = (hash * 33u) ^ byte;
    }
    return hash == 0 ? 1u : hash;
}

std::uint32_t name_hash_inline(const char* name) noexcept {
    std::uint32_t hash = kSeed;
    for (const unsigned char* p = reinterpret_cast<const unsigned char*>(name); *p != 0; ++p) {
        hash = (hash * 33u) ^ static_cast<std::uint32_t>(*p);
    }
    return hash;
}

}  // namespace sotc
