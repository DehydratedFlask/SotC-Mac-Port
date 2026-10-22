// Init `0x531d60` continue OBS `0x531e4b..0x531ece` (131 B): ymm BSS seed
//   vmovups ymm0, [0x01014a80]          ; (-1,0,0,0, 1,0,0,0) f32
//   lea rsi/rcx/rdi/rax → BSS cells
//   vmovups [0x1604278], ymm0 + qwords @+0x20/+0x28
//   dword stores @ 0x160426c (+0/+4/+8)
//   vmovups [0x16042dc], ymm0 + dwords @+0x20..+0x2c
//   mov dword [0x1604268], 0x3f800000
//   // stop before inter-cell ymm copies @ 0x531ece
//
// Always-on after E456 0xf90310 call-site. Prefer this 131 B Init continue AVX
// seed over helper-body tourism (0x26800 / 0xf590 string helpers later in the
// block) and over deepen 0x1290 / 0x5c64c0 / 0x56dcb0 / 0x499f80 / 0x140d90 /
// 0x14c2f0 / 0x6a5370. Prefer over staffroll 0x7f7220 body / empty-path
// migrate/non-null.
//
// Claimed: 32 B seed from 0x1014a80 into BSS cells + scalar fills listed above.
// NOT modelled: lea rbx/xor edx (later-helper prep); lea 0x16042a8 (next-frag
// prep); inter-cell ymm @ 0x531ece+; calls 0x26800 / 0xf590. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseYmmBssRva = 0x531e4b;
inline constexpr std::size_t kInit531d60PostBaseYmmBssSize = 131;
inline constexpr std::uint64_t kInit531d60PostBaseYmmBssEndRva = 0x531ece;
inline constexpr std::uint64_t kInit531d60PostBaseYmmBssSrcRva = 0x1014a80;
inline constexpr std::uint64_t kInit531d60PostBaseYmmBssDst4278Rva = 0x1604278;
inline constexpr std::uint64_t kInit531d60PostBaseYmmBssDst426cRva = 0x160426c;
inline constexpr std::uint64_t kInit531d60PostBaseYmmBssDst42dcRva = 0x16042dc;
inline constexpr std::uint64_t kInit531d60PostBaseYmmBssDst4268Rva = 0x1604268;
inline constexpr std::size_t kInit531d60PostBaseYmmBssLaneBytes = 32;
inline constexpr std::size_t kInit531d60PostBaseYmmBssDst4278Bytes = 0x30;
inline constexpr std::size_t kInit531d60PostBaseYmmBssDst42dcBytes = 0x30;
inline constexpr std::uint32_t kInit531d60PostBaseYmmBssF32One = 0x3f800000u;
inline constexpr std::uint32_t kInit531d60PostBaseYmmBssF32NegOne = 0xbf800000u;

// Host-backed cells for OBS BSS addresses (sizes as above).
struct Init531d60PostBaseYmmBssCells {
    std::uint8_t* src_1014a80 = nullptr;     // ≥ 32 B
    std::uint8_t* dst_1604278 = nullptr;     // ≥ 0x30 B
    std::uint8_t* dst_160426c = nullptr;     // ≥ 12 B
    std::uint8_t* dst_16042dc = nullptr;     // ≥ 0x30 B
    std::uint8_t* dst_1604268 = nullptr;     // ≥ 4 B
};

struct Init531d60PostBaseYmmBssResult {
    bool ok = false;
    int store_count = 0;  // 4 on success (one per destination region)
};

// Models OBS 0x531e4b..0x531ece on measured Init success path.
[[nodiscard]] Init531d60PostBaseYmmBssResult init_531d60_post_base_ymm_bss(
    const Init531d60PostBaseYmmBssCells& cells = {}) noexcept;

}  // namespace sotc
