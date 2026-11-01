// Init `0x531d60` registry mov rbx,[r13+0x10] OBS `0x532b70..0x532b74` (4 B):
//   mov rbx,[r13+0x10]   ; fall-through after lea rsi @ `0x532b6a`
//
// After E570 lea @ `0x532b6a`. NOT modelled: cmp/load tail @ `0x532b74..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRbxR13_10_532b70_U1Rva = 0x532b70;
inline constexpr std::size_t kInit531d60PostBaseMovRbxR13_10_532b70_U1Size = 4;
inline constexpr std::uint64_t kInit531d60PostBaseMovRbxR13_10_532b70_U1EndRva = 0x532b74;
inline constexpr std::uint32_t kInit531d60PostBaseMovRbxR13_10_532b70_U1Disp = 0x10;

struct Init531d60PostBaseMovRbxR13_10_532b70_U1Cells {
    const void* r13_base = nullptr;
    void** rbx_out = nullptr;
};

struct Init531d60PostBaseMovRbxR13_10_532b70_U1Result {
    bool ok = false;
    int mov_rbx_r13_10_count = 0;
    void* rbx = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRbxR13_10_532b70_U1Result init_531d60_post_base_mov_rbx_r13_10_532b70_u1(
    const Init531d60PostBaseMovRbxR13_10_532b70_U1Cells& cells = {}) noexcept;

}  // namespace sotc
