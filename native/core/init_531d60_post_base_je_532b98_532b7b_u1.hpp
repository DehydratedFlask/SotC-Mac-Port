// Init `0x531d60` registry je loop-exit OBS `0x532b7b..0x532b7d` (2 B rel8):
//   je 0x532b98   ; ZF from cmp r15,r11 @ `0x532b78`
//
// After E573 cmp @ `0x532b78`. NOT modelled: fall-through @ `0x532b7d..` or target @
// `0x532b98..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseJe532b98_532b7b_U1Rva = 0x532b7b;
inline constexpr std::size_t kInit531d60PostBaseJe532b98_532b7b_U1Size = 2;
inline constexpr std::uint64_t kInit531d60PostBaseJe532b98_532b7b_U1EndRva = 0x532b7d;
inline constexpr std::uint64_t kInit531d60PostBaseJe532b98_532b7b_U1TargetRva = 0x532b98;

struct Init531d60PostBaseJe532b98_532b7b_U1Cells {
    bool pointers_equal = false;
    bool pointers_equal_known = false;
};

struct Init531d60PostBaseJe532b98_532b7b_U1Result {
    bool ok = false;
    int je_532b98_count = 0;
    bool branch_outcome_modelled = false;
    bool branch_taken = false;
    std::uint64_t branch_target_rva = kInit531d60PostBaseJe532b98_532b7b_U1TargetRva;
};

[[nodiscard]] Init531d60PostBaseJe532b98_532b7b_U1Result init_531d60_post_base_je_532b98_532b7b_u1(
    const Init531d60PostBaseJe532b98_532b7b_U1Cells& cells = {}) noexcept;

}  // namespace sotc
