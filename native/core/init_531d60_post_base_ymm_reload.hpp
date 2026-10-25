// Init `0x531d60` continue OBS `0x531f3c..0x531f71` (53 B): ymm reload→0x160433c
//   // enters with rsi=0x160430c, rdi=0x160433c (from E458/E459)
//   mov ecx, 4
//   vmovdqu ymm1, [rsi+0x10]; vmovups ymm0, [rsi]
//   lea rsi, [0x1014cf2]  ; "CP10" — stack prep for later 0x26800 (NOT modelled)
//   mov [rsp+…], …; mov rsi, rbx; …
//   vmovdqu [rdi+0x10], ymm1; vmovups [rdi], ymm0   ; → 0x160433c (0x30 B)
//   // stop before vxorps / call 0x26800 @ 0x531f86
//
// Always-on after E459 xmm BSS seed. Prefer this 53 B AVX-class Init continue
// (BSS reload copy) over helper-body tourism (0x26800 / 0xf590 / CP10 string
// setup as a claimed API) and over deepen 0x1290 / 0x5c64c0 / 0x56dcb0 /
// 0x499f80 / 0x140d90 / 0x14c2f0 / 0x6a5370. Prefer over staffroll 0x7f7220
// body / empty-path migrate/non-null.
//
// Claimed: 0x30 B copy 0x160430c → 0x160433c.
// NOT modelled: CP10/stack materialization; mov ecx,4 as call arg; calls
// 0x26800 / 0xf590. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseYmmReloadRva = 0x531f3c;
inline constexpr std::size_t kInit531d60PostBaseYmmReloadSize = 53;
inline constexpr std::uint64_t kInit531d60PostBaseYmmReloadEndRva = 0x531f71;
inline constexpr std::uint64_t kInit531d60PostBaseYmmReloadSrc430cRva = 0x160430c;
inline constexpr std::uint64_t kInit531d60PostBaseYmmReloadDst433cRva = 0x160433c;
inline constexpr std::uint64_t kInit531d60PostBaseYmmReloadCp10Rva = 0x1014cf2;
inline constexpr std::size_t kInit531d60PostBaseYmmReloadBlockBytes = 0x30;

// Host-backed cells (each ≥ 0x30 bytes). Source is E459-updated 0x160430c.
struct Init531d60PostBaseYmmReloadCells {
    std::uint8_t* src_160430c = nullptr;
    std::uint8_t* dst_160433c = nullptr;
};

struct Init531d60PostBaseYmmReloadResult {
    bool ok = false;
    int store_count = 0;  // 1 on success (one destination region)
};

// Models OBS 0x531f3c..0x531f71 on measured Init success path.
[[nodiscard]] Init531d60PostBaseYmmReloadResult init_531d60_post_base_ymm_reload(
    const Init531d60PostBaseYmmReloadCells& cells = {}) noexcept;

}  // namespace sotc
