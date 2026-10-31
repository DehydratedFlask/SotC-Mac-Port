// Init `0x531d60` continue E lea OBS `0x532a7d..0x532a84` (7 B):
//   lea rax,[0x5388e0]   ; thunk 1 address pin (ret no-op)
//
// After E524 `vmovq_532a75_u1` end @ `0x532a7d`. NOT modelled: following
// following lea rdx,[rsp+0x470] @ `0x532a84` (E526 peel). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLea5388e0_532a7d_U1Rva = 0x532a7d;
inline constexpr std::size_t kInit531d60PostBaseLea5388e0_532a7d_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseLea5388e0_532a7d_U1EndRva = 0x532a84;
inline constexpr std::uint64_t kInit531d60PostBaseLea5388e0_532a7d_U1TargetRva = 0x5388e0;

struct Init531d60PostBaseLea5388e0_532a7d_U1Cells {
    void** rax_out = nullptr;
};

struct Init531d60PostBaseLea5388e0_532a7d_U1Result {
    bool ok = false;
    int lea_rip_count = 0;
};

[[nodiscard]] Init531d60PostBaseLea5388e0_532a7d_U1Result init_531d60_post_base_lea_5388e0_532a7d_u1(
    const Init531d60PostBaseLea5388e0_532a7d_U1Cells& cells = {}) noexcept;

}  // namespace sotc
