// Init `0x531d60` continue E lea OBS `0x532a8c..0x532a93` (7 B):
//   lea rsi,[0x15c8e18]   ; vtable 2 (save-data ctor 3 no-op source)
//
// After E526 `lea_rsp470_532a84_u1` end @ `0x532a8c`. NOT modelled: following
// stack/descriptor prep @ `0x532a93`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRsi15c8e18_532a8c_U1Rva = 0x532a8c;
inline constexpr std::size_t kInit531d60PostBaseLeaRsi15c8e18_532a8c_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRsi15c8e18_532a8c_U1EndRva = 0x532a93;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRsi15c8e18_532a8c_U1TargetRva = 0x15c8e18;

struct Init531d60PostBaseLeaRsi15c8e18_532a8c_U1Cells {
    void** rsi_out = nullptr;
};

struct Init531d60PostBaseLeaRsi15c8e18_532a8c_U1Result {
    bool ok = false;
    int lea_rip_count = 0;
};

[[nodiscard]] Init531d60PostBaseLeaRsi15c8e18_532a8c_U1Result init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1(
    const Init531d60PostBaseLeaRsi15c8e18_532a8c_U1Cells& cells = {}) noexcept;

}  // namespace sotc
