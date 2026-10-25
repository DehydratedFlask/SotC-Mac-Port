// Init `0x531d60` continue OBS `0x531e17..0x531e3c` (37 B): AVX BSS xmm copy
//   lea rax, [0x01b1dd40]
//   lea rcx, [0x01b1dd00]
//   vmovups xmm0, [rax]
//   vmovups xmm1, [rax]
//   vmovups [rcx], xmm0
//   lea rcx, [0x01b1dd20]
//   vmovups [rcx], xmm1
//   // stop before call 0x7f7220 @ 0x531e3c
//
// Always-on after E452 0x5b3cf0 call-site. Prefer this 37 B Init continue over
// staffroll helper body 0x7f7220 (~108 B) and over deepen 0x1290 / 0x5c64c0 /
// 0x56dcb0 / 0x499f80 / 0x140d90 / 0x14c2f0 / 0x6a5370. Prefer over empty-path
// migrate/non-null.
//
// Claimed: 16-byte copy from cell 0x1b1dd40 into 0x1b1dd00 and 0x1b1dd20.
// NOT modelled: call 0x7f7220 / later Init continues. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseAvxBssRva = 0x531e17;
inline constexpr std::size_t kInit531d60PostBaseAvxBssSize = 37;
inline constexpr std::uint64_t kInit531d60PostBaseAvxBssEndRva = 0x531e3c;
inline constexpr std::uint64_t kInit531d60PostBaseAvxBssSrcRva = 0x1b1dd40;
inline constexpr std::uint64_t kInit531d60PostBaseAvxBssDst0Rva = 0x1b1dd00;
inline constexpr std::uint64_t kInit531d60PostBaseAvxBssDst1Rva = 0x1b1dd20;
inline constexpr std::size_t kInit531d60PostBaseAvxBssLaneBytes = 16;

// Host-backed cells for the three OBS BSS addresses (each ≥ 16 bytes).
struct Init531d60PostBaseAvxBssCells {
    std::uint8_t* src_1b1dd40 = nullptr;
    std::uint8_t* dst_1b1dd00 = nullptr;
    std::uint8_t* dst_1b1dd20 = nullptr;
};

struct Init531d60PostBaseAvxBssResult {
    bool ok = false;
    int store_count = 0;  // 2 on success (both destinations written)
};

// Models OBS 0x531e17..0x531e3c on measured Init success path.
[[nodiscard]] Init531d60PostBaseAvxBssResult init_531d60_post_base_avx_bss(
    const Init531d60PostBaseAvxBssCells& cells = {}) noexcept;

}  // namespace sotc
