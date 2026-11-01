// Init `0x531d60` registry mov r13,r15 OBS `0x532bc4..0x532bc7` (3 B):
//   mov r13,r15   ; fall-through after je @ `0x532bc2` when r13 != r14 @ `0x532bbf`
//
// After E594 je @ `0x532bc2`. NOT modelled: call @ `0x532bc7..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR13R15_532bc4_U1Rva = 0x532bc4;
inline constexpr std::size_t kInit531d60PostBaseMovR13R15_532bc4_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovR13R15_532bc4_U1EndRva = 0x532bc7;

struct Init531d60PostBaseMovR13R15_532bc4_U1Cells {
    std::uint64_t r15_in = 0;
    std::uint64_t* r13_out = nullptr;
};

struct Init531d60PostBaseMovR13R15_532bc4_U1Result {
    bool ok = false;
    int mov_r13_r15_count = 0;
    std::uint64_t r13 = 0;
};

[[nodiscard]] Init531d60PostBaseMovR13R15_532bc4_U1Result init_531d60_post_base_mov_r13_r15_532bc4_u1(
    const Init531d60PostBaseMovR13R15_532bc4_U1Cells& cells = {}) noexcept;

}  // namespace sotc
