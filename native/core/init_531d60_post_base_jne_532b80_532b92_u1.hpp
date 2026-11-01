// Init `0x531d60` registry jne loop-back OBS `0x532b92..0x532b94` (2 B rel8):
//   jne 0x532b80   ; ZF clear from cmp r15,rbx @ `0x532b8f`
//
// After E580 cmp @ `0x532b8f`. NOT modelled: fall-through @ `0x532b94..` or re-entry @
// `0x532b80..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseJne532b80_532b92_U1Rva = 0x532b92;
inline constexpr std::size_t kInit531d60PostBaseJne532b80_532b92_U1Size = 2;
inline constexpr std::uint64_t kInit531d60PostBaseJne532b80_532b92_U1EndRva = 0x532b94;
inline constexpr std::uint64_t kInit531d60PostBaseJne532b80_532b92_U1TargetRva = 0x532b80;

struct Init531d60PostBaseJne532b80_532b92_U1Cells {
    bool pointers_equal = false;
    bool pointers_equal_known = false;
};

struct Init531d60PostBaseJne532b80_532b92_U1Result {
    bool ok = false;
    int jne_532b80_count = 0;
    bool branch_outcome_modelled = false;
    bool branch_taken = false;
    std::uint64_t branch_target_rva = kInit531d60PostBaseJne532b80_532b92_U1TargetRva;
};

[[nodiscard]] Init531d60PostBaseJne532b80_532b92_U1Result init_531d60_post_base_jne_532b80_532b92_u1(
    const Init531d60PostBaseJne532b80_532b92_U1Cells& cells = {}) noexcept;

}  // namespace sotc
