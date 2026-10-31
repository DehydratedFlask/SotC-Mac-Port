// Init `0x531d60` continue E mov OBS `0x532aef..0x532af2` (3 B):
//   mov rdi,rbx   ; this = descriptor + 0x20 (lea_rbx @ 0x532aaa)
//
// After E539 `mov_rsp4b0_rax_532ae7_u1` end @ `0x532aef`. NOT modelled: following
// `call [rax+0x18]` @ `0x532af2` (continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRdiRbx_532aef_U1Rva = 0x532aef;
inline constexpr std::size_t kInit531d60PostBaseMovRdiRbx_532aef_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovRdiRbx_532aef_U1EndRva = 0x532af2;

struct Init531d60PostBaseMovRdiRbx_532aef_U1Cells {
    // Host stand-in for rbx (address of descriptor + 0x20). Null → counted, no rdi write.
    const void* rbx_in = nullptr;
    void** rdi_out = nullptr;
};

struct Init531d60PostBaseMovRdiRbx_532aef_U1Result {
    bool ok = false;
    int mov_rdi_rbx_count = 0;
    void* rdi = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRdiRbx_532aef_U1Result init_531d60_post_base_mov_rdi_rbx_532aef_u1(
    const Init531d60PostBaseMovRdiRbx_532aef_U1Cells& cells = {}) noexcept;

}  // namespace sotc
