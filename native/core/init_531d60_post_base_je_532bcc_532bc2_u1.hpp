// Init `0x531d60` registry je r13-null skip OBS `0x532bc2..0x532bc4` (2 B rel8):
//   je 0x532bcc   ; ZF from test r13,r13 @ `0x532bbf`
//
// After E593 cmp @ `0x532bbf`. NOT modelled: E595 mov @ `0x532bc4` / tail @ `0x532bc7..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseJe532bcc_532bc2_U1Rva = 0x532bc2;
inline constexpr std::size_t kInit531d60PostBaseJe532bcc_532bc2_U1Size = 2;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bcc_532bc2_U1EndRva = 0x532bc4;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bcc_532bc2_U1TargetRva = 0x532bcc;

struct Init531d60PostBaseJe532bcc_532bc2_U1Cells {
    bool r13_is_zero = false;
    bool r13_is_zero_known = false;
};

struct Init531d60PostBaseJe532bcc_532bc2_U1Result {
    bool ok = false;
    int je_532bcc_count = 0;
    bool branch_outcome_modelled = false;
    bool branch_taken = false;
    std::uint64_t branch_target_rva = kInit531d60PostBaseJe532bcc_532bc2_U1TargetRva;
};

[[nodiscard]] Init531d60PostBaseJe532bcc_532bc2_U1Result init_531d60_post_base_je_532bcc_532bc2_u1(
    const Init531d60PostBaseJe532bcc_532bc2_U1Cells& cells = {}) noexcept;

}  // namespace sotc
