// Init `0x531d60` continue E lea OBS `0x532af5..0x532afc` (7 B):
//   lea rax,[0x2eff450]   ; BSS vtable-object cell (E542 peel)
//
// After E541 `vcall18_532af2_u1` end @ `0x532af5`. NOT modelled: following
// tail through `0x532b0c` (continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRax2eff450_532af5_U1Rva = 0x532af5;
inline constexpr std::size_t kInit531d60PostBaseLeaRax2eff450_532af5_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRax2eff450_532af5_U1EndRva = 0x532afc;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRax2eff450_532af5_U1TargetRva = 0x2eff450;

struct Init531d60PostBaseLeaRax2eff450_532af5_U1Cells {
    void** rax_out = nullptr;
};

struct Init531d60PostBaseLeaRax2eff450_532af5_U1Result {
    bool ok = false;
    int lea_rip_count = 0;
};

[[nodiscard]] Init531d60PostBaseLeaRax2eff450_532af5_U1Result
init_531d60_post_base_lea_rax_2eff450_532af5_u1(
    const Init531d60PostBaseLeaRax2eff450_532af5_U1Cells& cells = {}) noexcept;

}  // namespace sotc
