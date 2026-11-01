// Init `0x531d60` registry mov OBS `0x532b9d..0x532ba0` (3 B):
//   mov rdi,rbx   ; fall-through from je @ `0x532b9b` when rbx non-null
//
// After E584 je @ `0x532b9b`. NOT modelled: `call 0x170c0` @ `0x532ba0` or epilogue @
// `0x532ba5`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRdiRbx_532b9d_U1Rva = 0x532b9d;
inline constexpr std::size_t kInit531d60PostBaseMovRdiRbx_532b9d_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovRdiRbx_532b9d_U1EndRva = 0x532ba0;

struct Init531d60PostBaseMovRdiRbx_532b9d_U1Cells {
    const void* rbx_in = nullptr;
    void** rdi_out = nullptr;
};

struct Init531d60PostBaseMovRdiRbx_532b9d_U1Result {
    bool ok = false;
    int mov_rdi_rbx_532b9d_count = 0;
    void* rdi = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRdiRbx_532b9d_U1Result init_531d60_post_base_mov_rdi_rbx_532b9d_u1(
    const Init531d60PostBaseMovRdiRbx_532b9d_U1Cells& cells = {}) noexcept;

}  // namespace sotc
