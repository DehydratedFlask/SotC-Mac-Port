// Init `0x531d60` continue E mov OBS `0x532ab2..0x532aba` (8 B):
//   mov [rsp+0x470],rdx   ; desc3[0x00] = descriptor base (self-pointer at head)
//
// After E531 `lea_rbx_rsp490_532aaa_u1` end @ `0x532ab2`. NOT modelled: following
// `vmovdqa [rsp+0x478],xmm0` @ `0x532aba` (E532+ continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRsp470Rdx_532ab2_U1Rva = 0x532ab2;
inline constexpr std::size_t kInit531d60PostBaseMovRsp470Rdx_532ab2_U1Size = 8;
inline constexpr std::uint64_t kInit531d60PostBaseMovRsp470Rdx_532ab2_U1EndRva = 0x532aba;
inline constexpr std::uint32_t kInit531d60PostBaseMovRsp470Rdx_532ab2_U1Disp = 0x470;

struct Init531d60PostBaseMovRsp470Rdx_532ab2_U1Cells {
    // Optional direct write target for desc3[0x00]. Null => counted, no write.
    void** head_out = nullptr;
    // rdx operand stand-in (descriptor base). When head_out is set and rdx_in is
    // null, no store occurs but the mov is still counted.
    void* rdx_in = nullptr;
};

struct Init531d60PostBaseMovRsp470Rdx_532ab2_U1Result {
    bool ok = false;
    int mov_rsp470_rdx_count = 0;
};

[[nodiscard]] Init531d60PostBaseMovRsp470Rdx_532ab2_U1Result
init_531d60_post_base_mov_rsp470_rdx_532ab2_u1(
    const Init531d60PostBaseMovRsp470Rdx_532ab2_U1Cells& cells = {}) noexcept;

}  // namespace sotc
