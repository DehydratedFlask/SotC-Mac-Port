// Init `0x531d60` registry mov rax,[rbx+0x48] OBS `0x532b80..0x532b84` (4 B):
//   mov rax,[rbx+0x48]   ; fall-through after nop @ `0x532b7d`
//
// After E575 nop @ `0x532b7d`. NOT modelled: lea rdi/load tail @ `0x532b84..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRaxRbx_48_532b80_U1Rva = 0x532b80;
inline constexpr std::size_t kInit531d60PostBaseMovRaxRbx_48_532b80_U1Size = 4;
inline constexpr std::uint64_t kInit531d60PostBaseMovRaxRbx_48_532b80_U1EndRva = 0x532b84;
inline constexpr std::uint32_t kInit531d60PostBaseMovRaxRbx_48_532b80_U1Disp = 0x48;

struct Init531d60PostBaseMovRaxRbx_48_532b80_U1Cells {
    const void* rbx_base = nullptr;
    void** rax_out = nullptr;
};

struct Init531d60PostBaseMovRaxRbx_48_532b80_U1Result {
    bool ok = false;
    int mov_rax_rbx_48_count = 0;
    void* rax = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRaxRbx_48_532b80_U1Result init_531d60_post_base_mov_rax_rbx_48_532b80_u1(
    const Init531d60PostBaseMovRaxRbx_48_532b80_U1Cells& cells = {}) noexcept;

}  // namespace sotc
