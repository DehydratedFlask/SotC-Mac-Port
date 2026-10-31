// Init `0x531d60` registry je same-pointer fast path OBS `0x532b47..0x532b4d` (6 B):
//   je 0x532bfb   ; ZF from cmp r12,r14 @ `0x532b44`
//
// After E561 cmp @ `0x532b44`. NOT modelled: `0x532bfb` successor or fall-through destroy/store @
// `0x532b4d..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseJe532bfb_532b47_U1Rva = 0x532b47;
inline constexpr std::size_t kInit531d60PostBaseJe532bfb_532b47_U1Size = 6;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bfb_532b47_U1EndRva = 0x532b4d;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bfb_532b47_U1TargetRva = 0x532bfb;

struct Init531d60PostBaseJe532bfb_532b47_U1Cells {
    bool pointers_equal = false;
    bool pointers_equal_known = false;
};

struct Init531d60PostBaseJe532bfb_532b47_U1Result {
    bool ok = false;
    int je_532bfb_count = 0;
    bool branch_outcome_modelled = false;
    bool branch_taken = false;
    std::uint64_t branch_target_rva = kInit531d60PostBaseJe532bfb_532b47_U1TargetRva;
};

[[nodiscard]] Init531d60PostBaseJe532bfb_532b47_U1Result init_531d60_post_base_je_532bfb_532b47_u1(
    const Init531d60PostBaseJe532bfb_532b47_U1Cells& cells = {}) noexcept;

}  // namespace sotc
