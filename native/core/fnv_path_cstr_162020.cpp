#include "fnv_path_cstr_162020.hpp"

namespace sotc {
namespace {

// ASCII stand-in for the imported _Getptoupper table (PLT 0xf90180). Bytes outside 'a'..'z'
// are returned unchanged; byte 0xff is handled by the caller to match the original cmpq shortcut.
[[nodiscard]] unsigned char ascii_toupper_byte(unsigned char c) noexcept {
    if (c >= static_cast<unsigned char>('a') && c <= static_cast<unsigned char>('z')) {
        return static_cast<unsigned char>(c - static_cast<unsigned char>('a') +
                                          static_cast<unsigned char>('A'));
    }
    return c;
}

}  // namespace

std::uint32_t fnv_path_cstr_162020(const char* s, std::uint32_t seed) noexcept {
    if (s == nullptr) {
        return 0;
    }
    std::uint32_t h = seed;
    for (; *s != '\0'; ++s) {
        const auto raw = static_cast<unsigned char>(*s);
        unsigned char folded = (raw == 0xffu) ? 0xffu : ascii_toupper_byte(raw);
        if (folded == static_cast<unsigned char>('\\')) {
            folded = static_cast<unsigned char>('/');
        }
        h = (h ^ static_cast<std::uint32_t>(folded)) * kFnvPrime162020;
    }
    return h;
}

std::uint32_t fnv_cstr_a2ddc0(const char* s) noexcept {
    return fnv_path_cstr_162020(s, kFnvOffsetBasisA2ddc0);
}

}  // namespace sotc
