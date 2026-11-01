// Init `0x531d60` registry lea rdi OBS `0x532b84..0x532b88` (4 B):
//   lea rdi,[rbx+0x28]   ; fall-through after mov rax,[rbx+0x48] @ `0x532b80`
//
// After E576 mov rax @ `0x532b80`. NOT modelled: vcall/load tail @ `0x532b88..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRdiRbx_28_532b84_U1Rva = 0x532b84;
inline constexpr std::size_t kInit531d60PostBaseLeaRdiRbx_28_532b84_U1Size = 4;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRdiRbx_28_532b84_U1EndRva = 0x532b88;
inline constexpr std::uint32_t kInit531d60PostBaseLeaRdiRbx_28_532b84_U1Disp = 0x28;

struct Init531d60PostBaseLeaRdiRbx_28_532b84_U1Cells {
    const void* rbx_base = nullptr;
    void** rdi_out = nullptr;
};

struct Init531d60PostBaseLeaRdiRbx_28_532b84_U1Result {
    bool ok = false;
    int lea_rdi_rbx_28_count = 0;
    void* rdi = nullptr;
};

[[nodiscard]] Init531d60PostBaseLeaRdiRbx_28_532b84_U1Result
init_531d60_post_base_lea_rdi_rbx_28_532b84_u1(
    const Init531d60PostBaseLeaRdiRbx_28_532b84_U1Cells& cells = {}) noexcept;

}  // namespace sotc
