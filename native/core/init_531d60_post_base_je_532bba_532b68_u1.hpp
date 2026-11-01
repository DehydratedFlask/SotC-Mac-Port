// Init `0x531d60` registry je destroy-skip OBS `0x532b68..0x532b6a` (2 B rel8):
//   je 0x532bba   ; ZF from cmp r13,r14 @ `0x532b65`
//
// After E568 cmp @ `0x532b65`. NOT modelled: destroy epilogue @ `0x532bba` or fall-through @
// `0x532b6a..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseJe532bba_532b68_U1Rva = 0x532b68;
inline constexpr std::size_t kInit531d60PostBaseJe532bba_532b68_U1Size = 2;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bba_532b68_U1EndRva = 0x532b6a;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bba_532b68_U1TargetRva = 0x532bba;

struct Init531d60PostBaseJe532bba_532b68_U1Cells {
    bool pointers_equal = false;
    bool pointers_equal_known = false;
};

struct Init531d60PostBaseJe532bba_532b68_U1Result {
    bool ok = false;
    int je_532bba_count = 0;
    bool branch_outcome_modelled = false;
    bool branch_taken = false;
    std::uint64_t branch_target_rva = kInit531d60PostBaseJe532bba_532b68_U1TargetRva;
};

[[nodiscard]] Init531d60PostBaseJe532bba_532b68_U1Result init_531d60_post_base_je_532bba_532b68_u1(
    const Init531d60PostBaseJe532bba_532b68_U1Cells& cells = {}) noexcept;

}  // namespace sotc
