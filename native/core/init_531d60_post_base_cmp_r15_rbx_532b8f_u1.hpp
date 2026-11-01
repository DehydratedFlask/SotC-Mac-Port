// Init `0x531d60` registry cmp r15,rbx OBS `0x532b8f..0x532b92` (3 B):
//   cmp r15,rbx   ; end marker vs walk pointer after add rbx,+0x58 @ `0x532b8b`
//
// After E579 add rbx @ `0x532b8b`. NOT modelled: jne @ `0x532b92..` (E581 peel). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCmpR15Rbx_532b8f_U1Rva = 0x532b8f;
inline constexpr std::size_t kInit531d60PostBaseCmpR15Rbx_532b8f_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseCmpR15Rbx_532b8f_U1EndRva = 0x532b92;

struct Init531d60PostBaseCmpR15Rbx_532b8f_U1Cells {
    void* r15_in = nullptr;
    void* rbx_in = nullptr;
};

struct Init531d60PostBaseCmpR15Rbx_532b8f_U1Result {
    bool ok = false;
    int cmp_r15_rbx_count = 0;
    bool pointers_equal = false;
};

[[nodiscard]] Init531d60PostBaseCmpR15Rbx_532b8f_U1Result init_531d60_post_base_cmp_r15_rbx_532b8f_u1(
    const Init531d60PostBaseCmpR15Rbx_532b8f_U1Cells& cells = {}) noexcept;

}  // namespace sotc
