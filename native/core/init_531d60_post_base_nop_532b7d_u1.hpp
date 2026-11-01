// Init `0x531d60` registry nop OBS `0x532b7d..0x532b80` (3 B):
//   nop   ; fall-through after je @ `0x532b7b` when r15 != r11
//
// After E574 je fall-through @ `0x532b7d`. NOT modelled: mov rax/load tail @ `0x532b80..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseNop532b7d_U1Rva = 0x532b7d;
inline constexpr std::size_t kInit531d60PostBaseNop532b7d_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseNop532b7d_U1EndRva = 0x532b80;

struct Init531d60PostBaseNop532b7d_U1Cells {};

struct Init531d60PostBaseNop532b7d_U1Result {
    bool ok = false;
    int nop_532b7d_count = 0;
};

[[nodiscard]] Init531d60PostBaseNop532b7d_U1Result init_531d60_post_base_nop_532b7d_u1(
    const Init531d60PostBaseNop532b7d_U1Cells& cells = {}) noexcept;

}  // namespace sotc
