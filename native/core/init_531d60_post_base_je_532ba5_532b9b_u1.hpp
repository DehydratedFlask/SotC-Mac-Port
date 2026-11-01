// Init `0x531d60` registry je null-rbx skip OBS `0x532b9b..0x532b9d` (2 B rel8):
//   je 0x532ba5   ; ZF from test rbx,rbx @ `0x532b98`
//
// After E583 test @ `0x532b98`. Fall-through `mov rdi,rbx` is E585 `mov_rdi_rbx_532b9d_u1`.
// NOT modelled: `0x532ba5` epilogue. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseJe532ba5_532b9b_U1Rva = 0x532b9b;
inline constexpr std::size_t kInit531d60PostBaseJe532ba5_532b9b_U1Size = 2;
inline constexpr std::uint64_t kInit531d60PostBaseJe532ba5_532b9b_U1EndRva = 0x532b9d;
inline constexpr std::uint64_t kInit531d60PostBaseJe532ba5_532b9b_U1TargetRva = 0x532ba5;

struct Init531d60PostBaseJe532ba5_532b9b_U1Cells {
    bool rbx_is_zero = false;
    bool rbx_is_zero_known = false;
};

struct Init531d60PostBaseJe532ba5_532b9b_U1Result {
    bool ok = false;
    int je_532ba5_count = 0;
    bool branch_outcome_modelled = false;
    bool branch_taken = false;
    std::uint64_t branch_target_rva = kInit531d60PostBaseJe532ba5_532b9b_U1TargetRva;
};

[[nodiscard]] Init531d60PostBaseJe532ba5_532b9b_U1Result init_531d60_post_base_je_532ba5_532b9b_u1(
    const Init531d60PostBaseJe532ba5_532b9b_U1Cells& cells = {}) noexcept;

}  // namespace sotc
