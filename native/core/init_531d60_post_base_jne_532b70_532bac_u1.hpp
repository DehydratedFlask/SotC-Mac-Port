// Init `0x531d60` registry jne loop-back OBS `0x532bac..0x532bae` (2 B rel8):
//   jne 0x532b70   ; ZF clear from cmp r13,r14 @ `0x532ba9`
//
// After E588 cmp @ `0x532ba9`. NOT modelled: re-entry @ `0x532b70..`. Fall-through @
// `0x532bae..` peeled in a sibling unit. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseJne532b70_532bac_U1Rva = 0x532bac;
inline constexpr std::size_t kInit531d60PostBaseJne532b70_532bac_U1Size = 2;
inline constexpr std::uint64_t kInit531d60PostBaseJne532b70_532bac_U1EndRva = 0x532bae;
inline constexpr std::uint64_t kInit531d60PostBaseJne532b70_532bac_U1TargetRva = 0x532b70;

struct Init531d60PostBaseJne532b70_532bac_U1Cells {
    bool pointers_equal = false;
    bool pointers_equal_known = false;
};

struct Init531d60PostBaseJne532b70_532bac_U1Result {
    bool ok = false;
    int jne_532b70_532bac_count = 0;
    bool branch_outcome_modelled = false;
    bool branch_taken = false;
    std::uint64_t branch_target_rva = kInit531d60PostBaseJne532b70_532bac_U1TargetRva;
};

[[nodiscard]] Init531d60PostBaseJne532b70_532bac_U1Result init_531d60_post_base_jne_532b70_532bac_u1(
    const Init531d60PostBaseJne532b70_532bac_U1Cells& cells = {}) noexcept;

}  // namespace sotc
