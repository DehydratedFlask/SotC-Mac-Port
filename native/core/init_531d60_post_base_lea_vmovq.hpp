// Init `0x531d60` continue OBS `0x532036..0x532045` (15 B): lea rbx / vmovq
//   lea rbx, [0x32f79a8]                 ; singleton/global cell address
//   vmovq xmm0, [0x15ffa38]              ; load 8 B constant into xmm0
//   // stop before lea rsi (stack prep → call 0x532ca0)
//
// Always-on after E464 `0x52eb50` call-site. Prefer this lea/vmovq Init
// continue over trampoline/`0x52eb50` body tourism, over `0x26800`/`0xf590`
// / staffroll bodies, and over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` /
// `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.
//
// Claimed: rbx cell-address pin + 8 B vmovq load from 0x15ffa38.
// NOT modelled: stack object materialization; call 0x532ca0 / later units.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaVmovqRva = 0x532036;
inline constexpr std::size_t kInit531d60PostBaseLeaVmovqSize = 15;
inline constexpr std::uint64_t kInit531d60PostBaseLeaVmovqEndRva = 0x532045;
inline constexpr std::uint64_t kInit531d60PostBaseLeaVmovqRbxCellRva = 0x32f79a8;
inline constexpr std::uint64_t kInit531d60PostBaseLeaVmovqSrcRva = 0x15ffa38;
inline constexpr std::size_t kInit531d60PostBaseLeaVmovqLaneBytes = 8;

// Host-backed cells (src ≥ 8 B; dst_xmm0 ≥ 8 B).
struct Init531d60PostBaseLeaVmovqCells {
    const void* src_15ffa38 = nullptr;
    void* dst_xmm0 = nullptr;
    // Optional: host stand-in for the lea rbx target cell. If both set,
    // *rbx_out = rbx_cell (models lea loading &0x32f79a8 into rbx).
    void* rbx_cell = nullptr;
    void** rbx_out = nullptr;
};

struct Init531d60PostBaseLeaVmovqResult {
    bool ok = false;
    int load_count = 0;
    int lea_count = 0;
};

// Models OBS 0x532036..0x532045 on measured Init success path.
[[nodiscard]] Init531d60PostBaseLeaVmovqResult init_531d60_post_base_lea_vmovq(
    const Init531d60PostBaseLeaVmovqCells& cells = {}) noexcept;

}  // namespace sotc
