// Init `0x531d60` continue OBS `0x531f07..0x531f3c` (53 B): xmm BSS seed
//   // enters with rcx=0x16042d8, rsi=0x160430c (from E458)
//   vmovups xmm0, [0x01014be0]          ; (0,1,0,0) f32
//   mov dword [rcx], 0x3f800000         ; → 0x16042d8
//   mov rcx, 0x3f80000000000000
//   mov dword [rsi], 0xbf800000         ; → 0x160430c
//   mov dword [rsi+4], 0; [rsi+8], 0
//   vmovups [rsi+0xc], xmm0
//   mov qword [rsi+0x1c], rcx
//   // stop before mov ecx,4 / ymm reload / "CP10" / 0x26800
//
// Always-on after E458 ymm inter-cell. Prefer this 53 B XMM-class Init continue
// over helper-body tourism (0x26800 / 0xf590 / CP10 string setup) and over
// deepen 0x1290 / 0x5c64c0 / 0x56dcb0 / 0x499f80 / 0x140d90 / 0x14c2f0 /
// 0x6a5370. Prefer over staffroll 0x7f7220 body / empty-path migrate/non-null.
//
// Claimed: dword @ 0x16042d8; overwrite head of 0x160430c (+0..+0x23) from
// constant 0x1014be0 + scalar fills.
// NOT modelled: ymm reload→0x160433c; CP10/stack; calls 0x26800 / 0xf590.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseXmmSeedRva = 0x531f07;
inline constexpr std::size_t kInit531d60PostBaseXmmSeedSize = 53;
inline constexpr std::uint64_t kInit531d60PostBaseXmmSeedEndRva = 0x531f3c;
inline constexpr std::uint64_t kInit531d60PostBaseXmmSeedSrcRva = 0x1014be0;
inline constexpr std::uint64_t kInit531d60PostBaseXmmSeedDst42d8Rva = 0x16042d8;
inline constexpr std::uint64_t kInit531d60PostBaseXmmSeedDst430cRva = 0x160430c;
inline constexpr std::size_t kInit531d60PostBaseXmmSeedLaneBytes = 16;
inline constexpr std::size_t kInit531d60PostBaseXmmSeedDst430cBytes = 0x24;
inline constexpr std::uint32_t kInit531d60PostBaseXmmSeedF32One = 0x3f800000u;
inline constexpr std::uint32_t kInit531d60PostBaseXmmSeedF32NegOne = 0xbf800000u;
inline constexpr std::uint64_t kInit531d60PostBaseXmmSeedQOneHi = 0x3f80000000000000ull;

// Host-backed cells (src ≥ 16 B; dst42d8 ≥ 4 B; dst430c ≥ 0x24 B).
struct Init531d60PostBaseXmmSeedCells {
    std::uint8_t* src_1014be0 = nullptr;
    std::uint8_t* dst_16042d8 = nullptr;
    std::uint8_t* dst_160430c = nullptr;
};

struct Init531d60PostBaseXmmSeedResult {
    bool ok = false;
    int store_count = 0;  // 2 on success (one per destination region)
};

// Models OBS 0x531f07..0x531f3c on measured Init success path.
[[nodiscard]] Init531d60PostBaseXmmSeedResult init_531d60_post_base_xmm_seed(
    const Init531d60PostBaseXmmSeedCells& cells = {}) noexcept;

}  // namespace sotc
