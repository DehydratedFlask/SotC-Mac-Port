// Init `0x531d60` continue E mov edx,2 OBS `0x532afe..0x532b03` (5 B):
//   mov edx,2   ; arg2 = 2 for 0x54b250 dispatch
//
// After E543 `xor_esi_532afc_u1` end @ `0x532afe`. NOT modelled: following
// tail through `0x532b0c` (continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovEdx2_532afe_U1Rva = 0x532afe;
inline constexpr std::size_t kInit531d60PostBaseMovEdx2_532afe_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovEdx2_532afe_U1EndRva = 0x532b03;
inline constexpr std::uint32_t kInit531d60PostBaseMovEdx2_532afe_U1Immediate = 2;

struct Init531d60PostBaseMovEdx2_532afe_U1Result {
    bool ok = false;
    int mov_edx_count = 0;
    std::uint32_t edx = 0;
};

[[nodiscard]] Init531d60PostBaseMovEdx2_532afe_U1Result
init_531d60_post_base_mov_edx_2_532afe_u1() noexcept;

}  // namespace sotc
