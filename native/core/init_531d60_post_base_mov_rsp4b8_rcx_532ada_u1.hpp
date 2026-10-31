// Init `0x531d60` continue E mov OBS `0x532ada..0x532ae2` (8 B):
//   mov [rsp+0x4B8],rcx   ; desc3[0x48] = 0x538870 (thunk 2 from lea @ 0x532aa3 peel)
//
// After E536 `lea_rsi_2efda50_532ad3_u1` end @ `0x532ada`. NOT modelled: following
// `call 0xa54920` @ `0x532ae2` (continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRsp4b8_Rcx_532ada_U1Rva = 0x532ada;
inline constexpr std::size_t kInit531d60PostBaseMovRsp4b8_Rcx_532ada_U1Size = 8;
inline constexpr std::uint64_t kInit531d60PostBaseMovRsp4b8_Rcx_532ada_U1EndRva = 0x532ae2;
inline constexpr std::uint32_t kInit531d60PostBaseMovRsp4b8_Rcx_532ada_U1Disp = 0x4b8;
inline constexpr std::uint64_t kInit531d60PostBaseMovRsp4b8_Rcx_532ada_U1DefaultThunkRva = 0x538870;

struct Init531d60PostBaseMovRsp4b8_Rcx_532ada_U1Cells {
    // Optional write target for desc3[0x48]. Null => counted, no write.
    void** thunk_out = nullptr;
    // rcx operand (thunk 2 pin). Null => default 0x538870 when thunk_out is set.
    void* rcx_in = nullptr;
};

struct Init531d60PostBaseMovRsp4b8_Rcx_532ada_U1Result {
    bool ok = false;
    int mov_rsp4b8_rcx_count = 0;
};

[[nodiscard]] Init531d60PostBaseMovRsp4b8_Rcx_532ada_U1Result
init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1(
    const Init531d60PostBaseMovRsp4b8_Rcx_532ada_U1Cells& cells = {}) noexcept;

}  // namespace sotc
