// Name lookup `0x1fc80` (143 B): walk 0x98-stride table [begin,end) for strcmp match.
//
// OBS:
//   begin=*[rdi]; end=*[rdi+8]; if begin==end → null
//   count = (end-begin) / 0x98  (via sar3 + magic 0x86bca1af286bca1b)
//   for each elem: name = *[elem+0x28] or empty cstr; call 0xf8f500(name, needle)
//   on match return elem; else null
//
// Always-on on measured vslot 0x533100 post-header (show_areaInfo). Also hooked from
// vslot 0x534130. PLT 0xf8f500 body NOT modelled (injectable strcmp).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kLookup1fc80Rva = 0x1fc80;
inline constexpr std::size_t kLookup1fc80Size = 143;
inline constexpr std::size_t kLookup1fc80Stride = 0x98;
inline constexpr std::size_t kLookup1fc80NameOff = 0x28;
inline constexpr std::uint64_t kLookup1fc80CountMagic = 0x86bca1af286bca1bull;

struct Lookup1fc80Hooks {
    // Models PLT 0xf8f500(a,b) → 0 on equal. Null → use std::strcmp.
    int (*strcmp_f8f500)(const char* a, const char* b) = nullptr;
};

// Models 0x1fc80(rdi=table{begin,end}, rsi=needle) → matching elem or nullptr.
[[nodiscard]] void* lookup_1fc80(void* table, const char* needle,
                                 const Lookup1fc80Hooks& hooks = {}) noexcept;

}  // namespace sotc
