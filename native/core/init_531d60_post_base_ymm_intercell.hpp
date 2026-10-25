// Init `0x531d60` continue OBS `0x531ece..0x531f07` (57 B): ymm inter-cell copy
//   // enters with rsi=0x1604278, rdi=0x16042dc, rcx=0x16042a8 (from E457)
//   vmovups ymm2, [rsi+0x10]; vmovups ymm1, [rsi]; vmovups ymm0, [rdi]
//   lea rsi, [0x160430c]
//   vmovups [rcx+0x10], ymm2; vmovups [rcx], ymm1   ; → 0x16042a8 (0x30 B)
//   vmovups ymm1, [rdi+0x10]
//   lea rcx, [0x16042d8]; lea rdi, [0x160433c]     ; next-frag prep
//   vmovups [rsi+0x10], ymm1; vmovups [rsi], ymm0   ; → 0x160430c (0x30 B)
//   // stop before vmovups xmm from 0x1014be0 @ 0x531f07
//
// Always-on after E457 ymm BSS seed. Prefer this 57 B AVX-class Init continue
// over helper-body tourism (0x26800 / 0xf590) and over deepen 0x1290 /
// 0x5c64c0 / 0x56dcb0 / 0x499f80 / 0x140d90 / 0x14c2f0 / 0x6a5370. Prefer over
// staffroll 0x7f7220 body / empty-path migrate/non-null.
//
// Claimed: 0x30 B copy 0x1604278→0x16042a8 and 0x16042dc→0x160430c.
// NOT modelled: lea 0x16042d8 / 0x160433c (later prep); xmm seed @ 0x531f07+;
// calls 0x26800 / 0xf590. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseYmmIntercellRva = 0x531ece;
inline constexpr std::size_t kInit531d60PostBaseYmmIntercellSize = 57;
inline constexpr std::uint64_t kInit531d60PostBaseYmmIntercellEndRva = 0x531f07;
inline constexpr std::uint64_t kInit531d60PostBaseYmmIntercellSrc4278Rva = 0x1604278;
inline constexpr std::uint64_t kInit531d60PostBaseYmmIntercellSrc42dcRva = 0x16042dc;
inline constexpr std::uint64_t kInit531d60PostBaseYmmIntercellDst42a8Rva = 0x16042a8;
inline constexpr std::uint64_t kInit531d60PostBaseYmmIntercellDst430cRva = 0x160430c;
inline constexpr std::size_t kInit531d60PostBaseYmmIntercellBlockBytes = 0x30;

// Host-backed cells (each ≥ 0x30 bytes). Sources are E457 seed destinations.
struct Init531d60PostBaseYmmIntercellCells {
    std::uint8_t* src_1604278 = nullptr;
    std::uint8_t* src_16042dc = nullptr;
    std::uint8_t* dst_16042a8 = nullptr;
    std::uint8_t* dst_160430c = nullptr;
};

struct Init531d60PostBaseYmmIntercellResult {
    bool ok = false;
    int store_count = 0;  // 2 on success (one per destination region)
};

// Models OBS 0x531ece..0x531f07 on measured Init success path.
[[nodiscard]] Init531d60PostBaseYmmIntercellResult init_531d60_post_base_ymm_intercell(
    const Init531d60PostBaseYmmIntercellCells& cells = {}) noexcept;

}  // namespace sotc
