// Native lift of the ResourceSystem platform-tag lookup `0x147e20` (E589).
//
// Body `0x147e20..0x147ef0`, 208 B, sha256 `400cd009…3b8abe`: two `std::map<u32,Entry>`
// `lower_bound` descents, no call, no store, no allocation. The entry's tag is at `value+0x58`;
// `0x144640` writes `"_cmn"` by default and five registrations override it with `"_ps4"`.
// `0x147e10` (4 B, `89 77 40 c3`) sets the default key; its only caller is `0x533430` with
// `esi = 0x2a2a2a2a` (`"****"`).
//
// Lookup: exact fourcc -> its tag; else the default key `"****"` -> its tag; else `"_ps4"`.
// With the 25 rows below, every unregistered fourcc resolves to `"_cmn"` after population.
// The nine `0x533100` sites run after the registrations, so their honest tag is `"_cmn"`, not
// `"_ps4"`. An unpopulated registry falls back to `"_ps4"`.
//
// The tags are compile-time constants (rodata literals), so a static table is honest. The RB-tree
// order is not observable for this lookup; a scan/`lower_bound` over the table is equivalent.
//
// NOT modelled: the 0xe0 B map node, the `0x144640`/`0x1497f0` constructors, and the out-of-scope
// `'cwks'` registration into `[0x2ee2a50]`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kPlatformTagLookup147e20Rva = 0x147E20;
inline constexpr std::size_t kPlatformTagLookup147e20Size = 208;
inline constexpr std::uint64_t kPlatformTagLookup147e20EndRva = 0x147EF0;
inline constexpr std::uint64_t kPlatformDefaultKeySetterRva = 0x147E10;
inline constexpr std::uint64_t kResourceSystemCellRva = 0x2EE2D00;

// Little-endian dword values of the four tag bytes: "_cmn" = 5f 63 6d 6e, "_ps4" = 5f 70 73 34.
inline constexpr std::uint32_t kPlatformTagCmn = 0x6E6D635F;
inline constexpr std::uint32_t kPlatformTagPs4 = 0x3473705F;
inline constexpr std::uint32_t kPlatformWildcardFourcc = 0x2A2A2A2A;  // "****"

enum class PlatformRegistryState : std::uint8_t {
    kUninitialised = 0,  // empty registry -> hardcoded "_ps4"
    kPopulated = 1,      // 25 rows installed -> exact/wildcard/default
};

struct PlatformTagRow {
    std::uint32_t fourcc;
    std::uint32_t tag;
};

inline constexpr std::size_t kPlatformTagRowCount = 25;

// `fourcc("crmp") == 0x63726d70`, the value `movbe` produces from the literal's last four bytes.
[[nodiscard]] constexpr std::uint32_t fourcc_of(const char (&s)[5]) noexcept {
    return (static_cast<std::uint32_t>(static_cast<unsigned char>(s[0])) << 24) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(s[1])) << 16) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(s[2])) << 8) |
           static_cast<std::uint32_t>(static_cast<unsigned char>(s[3]));
}

// The 24 registered fourccs plus the `"****"` default row. 22 rows are `_cmn`, 3 are `_ps4`
// (`cfon`, `cptx`, `cctx`), and the wildcard is `_cmn`.
extern const PlatformTagRow kPlatformTagRows[kPlatformTagRowCount];

// `0x147e20(fourcc, registry)`. `kPopulated` is the state at the nine attach sites.
[[nodiscard]] std::uint32_t platform_tag_147e20(
    std::uint32_t fourcc, PlatformRegistryState state = PlatformRegistryState::kPopulated) noexcept;

// One-argument adapter for the attach fragments' `platform_tag` hook (populated registry).
[[nodiscard]] std::uint32_t platform_tag_147e20_default(std::uint32_t fourcc) noexcept;

}  // namespace sotc
