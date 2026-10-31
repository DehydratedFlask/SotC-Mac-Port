// Init `0x531d60` continue E lea OBS `0x532a84..0x532a8c` (8 B):
//   lea rdx,[rsp+0x470]   ; save-data descriptor 3 stack slot
//
// After E525 `lea_5388e0_532a7d_u1` end @ `0x532a84`. NOT modelled: following
// vtable/thunk prep @ `0x532a8c`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRsp470_532a84_U1Rva = 0x532a84;
inline constexpr std::size_t kInit531d60PostBaseLeaRsp470_532a84_U1Size = 8;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRsp470_532a84_U1EndRva = 0x532a8c;
inline constexpr std::uint32_t kInit531d60PostBaseLeaRsp470_532a84_U1Disp = 0x470;

struct Init531d60PostBaseLeaRsp470_532a84_U1Cells {
    const void* stack_rsp = nullptr;
    void** rdx_out = nullptr;
};

struct Init531d60PostBaseLeaRsp470_532a84_U1Result {
    bool ok = false;
    int lea_rsp_count = 0;
};

[[nodiscard]] Init531d60PostBaseLeaRsp470_532a84_U1Result init_531d60_post_base_lea_rsp470_532a84_u1(
    const Init531d60PostBaseLeaRsp470_532a84_U1Cells& cells = {}) noexcept;

}  // namespace sotc
