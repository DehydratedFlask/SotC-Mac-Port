// Init `0x531d60` registry cmp r12,r14 OBS `0x532b44..0x532b47` (3 B):
//   cmp r12,r14   ; previous cell pointer vs alloc result saved in r14 @ 0x532b16
//
// After E560 mov r12,[rbx] @ `0x532b41`. NOT modelled: destroy-store fall-through @ `0x532b4d..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCmpR12R14_532b44_U1Rva = 0x532b44;
inline constexpr std::size_t kInit531d60PostBaseCmpR12R14_532b44_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseCmpR12R14_532b44_U1EndRva = 0x532b47;

struct Init531d60PostBaseCmpR12R14_532b44_U1Cells {
    void* r12_in = nullptr;
    void* r14_in = nullptr;
};

struct Init531d60PostBaseCmpR12R14_532b44_U1Result {
    bool ok = false;
    int cmp_r12_r14_count = 0;
    bool pointers_equal = false;
};

[[nodiscard]] Init531d60PostBaseCmpR12R14_532b44_U1Result init_531d60_post_base_cmp_r12_r14_532b44_u1(
    const Init531d60PostBaseCmpR12R14_532b44_U1Cells& cells = {}) noexcept;

}  // namespace sotc
