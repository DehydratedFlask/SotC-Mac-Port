// Init `0x531d60` registry mov edi,0x98 OBS `0x532b0c..0x532b11` (5 B):
//   mov edi,0x98   ; size arg for call 0x17070
//
// After E547 empty continue_e @ `0x532b0c`. NOT modelled: 0x17070 body. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovEdi98_532b0c_U1Rva = 0x532b0c;
inline constexpr std::size_t kInit531d60PostBaseMovEdi98_532b0c_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovEdi98_532b0c_U1EndRva = 0x532b11;
inline constexpr std::uint32_t kInit531d60PostBaseMovEdi98_532b0c_U1Immediate = 0x98;

struct Init531d60PostBaseMovEdi98_532b0c_U1Result {
    bool ok = false;
    int mov_edi_count = 0;
    std::uint32_t edi = 0;
};

[[nodiscard]] Init531d60PostBaseMovEdi98_532b0c_U1Result
init_531d60_post_base_mov_edi_98_532b0c_u1() noexcept;

}  // namespace sotc
