// Init `0x531d60` registry mov r15,rax OBS `0x532b26..0x532b29` (3 B):
//   mov r15,rax   ; save count from call 0x54dc20 @ 0x532b21 (eax = 0x113)
//
// After E553 call @ `0x532b21`. NOT modelled: store/compare @ `0x532b33..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR15Rax_532b26_U1Rva = 0x532b26;
inline constexpr std::size_t kInit531d60PostBaseMovR15Rax_532b26_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovR15Rax_532b26_U1EndRva = 0x532b29;

struct Init531d60PostBaseMovR15Rax_532b26_U1Cells {
    // Zero-extended eax from 0x54dc20. Default 0 when omitted by host.
    std::uint64_t rax_in = 0;
    std::uint64_t* r15_out = nullptr;
};

struct Init531d60PostBaseMovR15Rax_532b26_U1Result {
    bool ok = false;
    int mov_r15_rax_count = 0;
    std::uint64_t r15 = 0;
};

[[nodiscard]] Init531d60PostBaseMovR15Rax_532b26_U1Result init_531d60_post_base_mov_r15_rax_532b26_u1(
    const Init531d60PostBaseMovR15Rax_532b26_U1Cells& cells = {}) noexcept;

}  // namespace sotc
