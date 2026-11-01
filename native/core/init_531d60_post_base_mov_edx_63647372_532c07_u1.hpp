// Init `0x531d60` registry mov edx,0x63647372 OBS `0x532c07..0x532c0c` (5 B):
//   mov edx,0x63647372   ; tag immediate after E610 mov r14,[rsp+0x20] @ `0x532c02`
//
// After E610 mov @ `0x532c02`. NOT modelled: tail @ `0x532c0c`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovEdx63647372_532c07_U1Rva = 0x532c07;
inline constexpr std::size_t kInit531d60PostBaseMovEdx63647372_532c07_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovEdx63647372_532c07_U1EndRva = 0x532c0c;
inline constexpr std::uint32_t kInit531d60PostBaseMovEdx63647372_532c07_U1Immediate = 0x63647372u;

struct Init531d60PostBaseMovEdx63647372_532c07_U1Result {
    bool ok = false;
    int mov_edx_63647372_532c07_count = 0;
    std::uint32_t edx = 0;
};

[[nodiscard]] Init531d60PostBaseMovEdx63647372_532c07_U1Result
init_531d60_post_base_mov_edx_63647372_532c07_u1() noexcept;

}  // namespace sotc
