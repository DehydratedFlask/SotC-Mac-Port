// Init `0x531d60` continue E vmovdqa OBS `0x532aba..0x532ac3` (9 B):
//   vmovdqa [rsp+0x478],xmm0   ; desc3[0x08] = xmm0 (from vmovq @ 0x532a75 peel)
//
// After E532 `mov_rsp470_rdx_532ab2_u1` end @ `0x532aba`. NOT modelled: following
// `mov [rsp+0x488],rax` @ `0x532ac3` (E534 mov_rsp488 u1). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Rva = 0x532aba;
inline constexpr std::size_t kInit531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Size = 9;
inline constexpr std::uint64_t kInit531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1EndRva = 0x532ac3;
inline constexpr std::uint32_t kInit531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Disp = 0x478;
inline constexpr std::size_t kInit531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1LaneBytes = 16;

struct Init531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Cells {
    // 16-byte xmm0 source (typically vmovq output). Null => counted, zero stand-in.
    const std::uint8_t* xmm0_in = nullptr;
    // Optional write target for desc3[0x08]. Null => counted, no write.
    std::uint8_t* dest_out = nullptr;
};

struct Init531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Result {
    bool ok = false;
    int vmovdqa_count = 0;
};

[[nodiscard]] Init531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Result
init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1(
    const Init531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Cells& cells = {}) noexcept;

}  // namespace sotc
