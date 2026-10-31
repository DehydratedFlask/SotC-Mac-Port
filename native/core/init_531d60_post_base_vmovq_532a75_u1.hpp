// Init `0x531d60` continue E vmovq OBS `0x532a75..0x532a7d` (8 B):
//   vmovq xmm0,[0x15ff528]   ; 8 B rodata load for descriptor block 3
//
// After E523 `vcall18_532a72_u1` end @ `0x532a75`. No prep demoted in this u1.
// NOT modelled: following lea/descriptor stack prep @ `0x532a7d`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseVmovq532a75_U1Rva = 0x532a75;
inline constexpr std::size_t kInit531d60PostBaseVmovq532a75_U1Size = 8;
inline constexpr std::uint64_t kInit531d60PostBaseVmovq532a75_U1EndRva = 0x532a7d;
inline constexpr std::uint64_t kInit531d60PostBaseVmovq532a75_U1SrcRva = 0x15ff528;
inline constexpr std::size_t kInit531d60PostBaseVmovq532a75_U1LaneBytes = 8;

struct Init531d60PostBaseVmovq532a75_U1Cells {
    const void* src_15ff528 = nullptr;
    void* dst_xmm0 = nullptr;
};

struct Init531d60PostBaseVmovq532a75_U1Result {
    bool ok = false;
    int vmovq_count = 0;
};

[[nodiscard]] Init531d60PostBaseVmovq532a75_U1Result init_531d60_post_base_vmovq_532a75_u1(
    const Init531d60PostBaseVmovq532a75_U1Cells& cells = {}) noexcept;

}  // namespace sotc
