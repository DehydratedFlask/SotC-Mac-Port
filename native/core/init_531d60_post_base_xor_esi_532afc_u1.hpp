// Init `0x531d60` continue E xor OBS `0x532afc..0x532afe` (2 B):
//   xor esi,esi   ; arg1 = 0 for 0x54b250 dispatch
//
// After E542 `lea_rax_2eff450_532af5_u1` end @ `0x532afc`. NOT modelled: following
// tail through `0x532b0c` (continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseXorEsi_532afc_U1Rva = 0x532afc;
inline constexpr std::size_t kInit531d60PostBaseXorEsi_532afc_U1Size = 2;
inline constexpr std::uint64_t kInit531d60PostBaseXorEsi_532afc_U1EndRva = 0x532afe;

struct Init531d60PostBaseXorEsi_532afc_U1Result {
    bool ok = false;
    int xor_esi_count = 0;
    std::uint32_t esi = 0;
};

[[nodiscard]] Init531d60PostBaseXorEsi_532afc_U1Result init_531d60_post_base_xor_esi_532afc_u1() noexcept;

}  // namespace sotc
