// Init `0x531d60` continue E mov OBS `0x532acb..0x532ad3` (8 B):
//   mov [rsp+0x4B0],rsi   ; desc3 stack slot = vtable 2 (0x15c8e18 from lea @ 0x532a8c peel)
//
// After E534 `mov_rsp488_rax_532ac3_u1` end @ `0x532acb`. NOT modelled: following
// `lea rsi,[0x2efda50]` @ `0x532ad3` (E535+ continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRsp4b0_Rsi_532acb_U1Rva = 0x532acb;
inline constexpr std::size_t kInit531d60PostBaseMovRsp4b0_Rsi_532acb_U1Size = 8;
inline constexpr std::uint64_t kInit531d60PostBaseMovRsp4b0_Rsi_532acb_U1EndRva = 0x532ad3;
inline constexpr std::uint32_t kInit531d60PostBaseMovRsp4b0_Rsi_532acb_U1Disp = 0x4b0;
inline constexpr std::uint64_t kInit531d60PostBaseMovRsp4b0_Rsi_532acb_U1DefaultVtableRva =
    0x15c8e18;

struct Init531d60PostBaseMovRsp4b0_Rsi_532acb_U1Cells {
    // Optional write target for the stack vtable slot (desc3[0x40]). Null => counted, no write.
    void** vtable_stack_out = nullptr;
    // rsi operand (vtable 2 pin). Null => default 0x15c8e18 when vtable_stack_out is set.
    void* rsi_in = nullptr;
};

struct Init531d60PostBaseMovRsp4b0_Rsi_532acb_U1Result {
    bool ok = false;
    int mov_rsp4b0_rsi_count = 0;
};

[[nodiscard]] Init531d60PostBaseMovRsp4b0_Rsi_532acb_U1Result
init_531d60_post_base_mov_rsp4b0_rsi_532acb_u1(
    const Init531d60PostBaseMovRsp4b0_Rsi_532acb_U1Cells& cells = {}) noexcept;

}  // namespace sotc
