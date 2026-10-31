// Init `0x531d60` continue E mov OBS `0x532a93..0x532a9f` (12 B):
//   mov qword [rsp+0x4A0],0   ; save-data descriptor 3 field @ +0x30
//
// After E527 `lea_rsi_15c8e18_532a8c_u1` end @ `0x532a93`. NOT modelled: following
// `mov rdi,[r13]` @ `0x532a9f`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovqRsp4a0_0_532a93_U1Rva = 0x532a93;
inline constexpr std::size_t kInit531d60PostBaseMovqRsp4a0_0_532a93_U1Size = 12;
inline constexpr std::uint64_t kInit531d60PostBaseMovqRsp4a0_0_532a93_U1EndRva = 0x532a9f;

struct Init531d60PostBaseMovqRsp4a0_0_532a93_U1Cells {
    std::uint64_t* field_30_out = nullptr;
};

struct Init531d60PostBaseMovqRsp4a0_0_532a93_U1Result {
    bool ok = false;
    int movq_zero_count = 0;
};

[[nodiscard]] Init531d60PostBaseMovqRsp4a0_0_532a93_U1Result init_531d60_post_base_movq_rsp4a0_0_532a93_u1(
    const Init531d60PostBaseMovqRsp4a0_0_532a93_U1Cells& cells = {}) noexcept;

}  // namespace sotc
