// Init `0x531d60` registry mov edx,eax OBS `0x532b2c..0x532b2e` (2 B):
//   mov edx,eax   ; third arg for call 0x52a220 (count from 0x54dc20 still in eax)
//
// After E555 mov rsi,rbx @ `0x532b29`. NOT modelled: call 0x52a220 @ `0x532b2e`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovEdxEax_532b2c_U1Rva = 0x532b2c;
inline constexpr std::size_t kInit531d60PostBaseMovEdxEax_532b2c_U1Size = 2;
inline constexpr std::uint64_t kInit531d60PostBaseMovEdxEax_532b2c_U1EndRva = 0x532b2e;

struct Init531d60PostBaseMovEdxEax_532b2c_U1Cells {
    std::uint32_t eax_in = 0;
    std::uint32_t* edx_out = nullptr;
};

struct Init531d60PostBaseMovEdxEax_532b2c_U1Result {
    bool ok = false;
    int mov_edx_eax_count = 0;
    std::uint32_t edx = 0;
};

[[nodiscard]] Init531d60PostBaseMovEdxEax_532b2c_U1Result init_531d60_post_base_mov_edx_eax_532b2c_u1(
    const Init531d60PostBaseMovEdxEax_532b2c_U1Cells& cells = {}) noexcept;

}  // namespace sotc
