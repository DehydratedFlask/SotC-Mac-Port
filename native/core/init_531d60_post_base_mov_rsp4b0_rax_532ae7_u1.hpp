// Init `0x531d60` continue E mov OBS `0x532ae7..0x532aef` (8 B):
//   mov rax,[rsp+0x4B0]   ; reload vtable 2 (0x15c8e18 from E535 store peel)
//
// After E538 `a54920_532ae2_u1` end @ `0x532ae7`. NOT modelled: following
// `mov rdi,rbx` @ `0x532aef` (continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRsp4b0_Rax_532ae7_U1Rva = 0x532ae7;
inline constexpr std::size_t kInit531d60PostBaseMovRsp4b0_Rax_532ae7_U1Size = 8;
inline constexpr std::uint64_t kInit531d60PostBaseMovRsp4b0_Rax_532ae7_U1EndRva = 0x532aef;
inline constexpr std::uint32_t kInit531d60PostBaseMovRsp4b0_Rax_532ae7_U1Disp = 0x4b0;
inline constexpr std::uint64_t kInit531d60PostBaseMovRsp4b0_Rax_532ae7_U1DefaultVtableRva =
    0x15c8e18;

struct Init531d60PostBaseMovRsp4b0_Rax_532ae7_U1Cells {
    // Optional read source for [rsp+0x4B0]. Null => default vtable pin when rax_out is set.
    const void** vtable_stack_in = nullptr;
    // Optional write target for loaded rax. Null => counted, no write.
    void** rax_out = nullptr;
};

struct Init531d60PostBaseMovRsp4b0_Rax_532ae7_U1Result {
    bool ok = false;
    int mov_rsp4b0_rax_count = 0;
};

[[nodiscard]] Init531d60PostBaseMovRsp4b0_Rax_532ae7_U1Result
init_531d60_post_base_mov_rsp4b0_rax_532ae7_u1(
    const Init531d60PostBaseMovRsp4b0_Rax_532ae7_U1Cells& cells = {}) noexcept;

}  // namespace sotc
