// Init `0x531d60` registry je null-r12 skip OBS `0x532b50..0x532b56` (6 B):
//   je 0x532bf8   ; ZF from test r12,r12 @ `0x532b4d`
//
// After E563 test @ `0x532b4d`. NOT modelled: E608 mov @ `0x532bf8` / E609 lea @ `0x532bfb` / E610 mov @ `0x532c02` or fall-through destroy @
// `0x532b56..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseJe532bf8_532b50_U1Rva = 0x532b50;
inline constexpr std::size_t kInit531d60PostBaseJe532bf8_532b50_U1Size = 6;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bf8_532b50_U1EndRva = 0x532b56;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bf8_532b50_U1TargetRva = 0x532bf8;

struct Init531d60PostBaseJe532bf8_532b50_U1Cells {
    bool r12_is_zero = false;
    bool r12_is_zero_known = false;
};

struct Init531d60PostBaseJe532bf8_532b50_U1Result {
    bool ok = false;
    int je_532bf8_count = 0;
    bool branch_outcome_modelled = false;
    bool branch_taken = false;
    std::uint64_t branch_target_rva = kInit531d60PostBaseJe532bf8_532b50_U1TargetRva;
};

[[nodiscard]] Init531d60PostBaseJe532bf8_532b50_U1Result init_531d60_post_base_je_532bf8_532b50_u1(
    const Init531d60PostBaseJe532bf8_532b50_U1Cells& cells = {}) noexcept;

}  // namespace sotc
