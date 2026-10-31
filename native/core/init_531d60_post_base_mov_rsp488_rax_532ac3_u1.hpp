// Init `0x531d60` continue E mov OBS `0x532ac3..0x532acb` (8 B):
//   mov [rsp+0x488],rax   ; desc3[0x18] = 0x5388e0 (thunk 1 from lea @ 0x532a7d peel)
//
// After E533 `vmovdqa_rsp478_xmm0_532aba_u1` end @ `0x532ac3`. NOT modelled: following
// `mov [rsp+0x4B0],rsi` @ `0x532acb` (E535 mov_rsp4b0 u1). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRsp488Rax_532ac3_U1Rva = 0x532ac3;
inline constexpr std::size_t kInit531d60PostBaseMovRsp488Rax_532ac3_U1Size = 8;
inline constexpr std::uint64_t kInit531d60PostBaseMovRsp488Rax_532ac3_U1EndRva = 0x532acb;
inline constexpr std::uint32_t kInit531d60PostBaseMovRsp488Rax_532ac3_U1Disp = 0x488;
inline constexpr std::uint64_t kInit531d60PostBaseMovRsp488Rax_532ac3_U1DefaultThunkRva = 0x5388e0;

struct Init531d60PostBaseMovRsp488Rax_532ac3_U1Cells {
    // Optional write target for desc3[0x18]. Null => counted, no write.
    void** thunk_out = nullptr;
    // rax operand (thunk 1 pin). Null => default 0x5388e0 when thunk_out is set.
    void* rax_in = nullptr;
};

struct Init531d60PostBaseMovRsp488Rax_532ac3_U1Result {
    bool ok = false;
    int mov_rsp488_rax_count = 0;
};

[[nodiscard]] Init531d60PostBaseMovRsp488Rax_532ac3_U1Result
init_531d60_post_base_mov_rsp488_rax_532ac3_u1(
    const Init531d60PostBaseMovRsp488Rax_532ac3_U1Cells& cells = {}) noexcept;

}  // namespace sotc
