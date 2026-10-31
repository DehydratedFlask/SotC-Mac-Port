// Init `0x531d60` registry mov rsi,rbx OBS `0x532b29..0x532b2c` (3 B):
//   mov rsi,rbx   ; table iterator base from mov rbx,rax @ 0x532b1e
//
// After E554 mov r15,rax @ `0x532b26`. NOT modelled: mov edx,eax @ `0x532b2c`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRsiRbx_532b29_U1Rva = 0x532b29;
inline constexpr std::size_t kInit531d60PostBaseMovRsiRbx_532b29_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovRsiRbx_532b29_U1EndRva = 0x532b2c;

struct Init531d60PostBaseMovRsiRbx_532b29_U1Cells {
    const void* rbx_in = nullptr;
    void** rsi_out = nullptr;
};

struct Init531d60PostBaseMovRsiRbx_532b29_U1Result {
    bool ok = false;
    int mov_rsi_rbx_count = 0;
    void* rsi = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRsiRbx_532b29_U1Result init_531d60_post_base_mov_rsi_rbx_532b29_u1(
    const Init531d60PostBaseMovRsiRbx_532b29_U1Cells& cells = {}) noexcept;

}  // namespace sotc
