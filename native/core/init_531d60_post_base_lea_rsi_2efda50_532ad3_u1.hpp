// Init `0x531d60` continue E lea OBS `0x532ad3..0x532ada` (7 B):
//   lea rsi,[0x2efda50]   ; name struct 3 ("save_%s_mem_used")
//
// After E535 `mov_rsp4b0_rsi_532acb_u1` end @ `0x532ad3`. NOT modelled: following
// `mov [rsp+0x4B8],rcx` @ `0x532ada` (E537 mov_rsp4b8 u1). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRsi2efda50_532ad3_U1Rva = 0x532ad3;
inline constexpr std::size_t kInit531d60PostBaseLeaRsi2efda50_532ad3_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRsi2efda50_532ad3_U1EndRva = 0x532ada;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRsi2efda50_532ad3_U1TargetRva = 0x2efda50;

struct Init531d60PostBaseLeaRsi2efda50_532ad3_U1Cells {
    void** rsi_out = nullptr;
};

struct Init531d60PostBaseLeaRsi2efda50_532ad3_U1Result {
    bool ok = false;
    int lea_rip_count = 0;
};

[[nodiscard]] Init531d60PostBaseLeaRsi2efda50_532ad3_U1Result
init_531d60_post_base_lea_rsi_2efda50_532ad3_u1(
    const Init531d60PostBaseLeaRsi2efda50_532ad3_U1Cells& cells = {}) noexcept;

}  // namespace sotc
