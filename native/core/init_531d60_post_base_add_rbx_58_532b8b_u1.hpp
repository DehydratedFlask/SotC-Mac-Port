// Init `0x531d60` registry add rbx OBS `0x532b8b..0x532b8f` (4 B):
//   add rbx,0x58   ; fall-through after call [rax+0x18] @ `0x532b88`
//
// After E578 vcall18 @ `0x532b88`. NOT modelled: cmp r15,rbx @ `0x532b8f..` (E580 sibling). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseAddRbx_58_532b8b_U1Rva = 0x532b8b;
inline constexpr std::size_t kInit531d60PostBaseAddRbx_58_532b8b_U1Size = 4;
inline constexpr std::uint64_t kInit531d60PostBaseAddRbx_58_532b8b_U1EndRva = 0x532b8f;
inline constexpr std::uint32_t kInit531d60PostBaseAddRbx_58_532b8b_U1Imm = 0x58;

struct Init531d60PostBaseAddRbx_58_532b8b_U1Cells {
    const void* rbx_in = nullptr;
    void** rbx_out = nullptr;
};

struct Init531d60PostBaseAddRbx_58_532b8b_U1Result {
    bool ok = false;
    int add_rbx_58_count = 0;
    void* rbx = nullptr;
};

[[nodiscard]] Init531d60PostBaseAddRbx_58_532b8b_U1Result init_531d60_post_base_add_rbx_58_532b8b_u1(
    const Init531d60PostBaseAddRbx_58_532b8b_U1Cells& cells = {}) noexcept;

}  // namespace sotc
