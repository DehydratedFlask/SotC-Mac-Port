// Init `0x531d60` continue E lea OBS `0x532aaa..0x532ab2` (8 B):
//   lea rbx,[rsp+0x490]   ; &desc3[0x20] (self-ptr for vcall dispatch)
//
// After E530 `lea_538870_532aa3_u1` end @ `0x532aaa`. NOT modelled: following
// `mov [rsp+0x470],rdx` @ `0x532ab2` (E531+ continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRbxRsp490_532aaa_U1Rva = 0x532aaa;
inline constexpr std::size_t kInit531d60PostBaseLeaRbxRsp490_532aaa_U1Size = 8;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRbxRsp490_532aaa_U1EndRva = 0x532ab2;
inline constexpr std::uint32_t kInit531d60PostBaseLeaRbxRsp490_532aaa_U1Disp = 0x490;

struct Init531d60PostBaseLeaRbxRsp490_532aaa_U1Cells {
    const void* stack_rsp = nullptr;
    void** rbx_out = nullptr;
};

struct Init531d60PostBaseLeaRbxRsp490_532aaa_U1Result {
    bool ok = false;
    int lea_rsp_count = 0;
};

[[nodiscard]] Init531d60PostBaseLeaRbxRsp490_532aaa_U1Result
init_531d60_post_base_lea_rbx_rsp490_532aaa_u1(
    const Init531d60PostBaseLeaRbxRsp490_532aaa_U1Cells& cells = {}) noexcept;

}  // namespace sotc
