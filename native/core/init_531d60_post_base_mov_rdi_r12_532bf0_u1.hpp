// Init `0x531d60` registry mov rdi,r12 OBS `0x532bf0..0x532bf3` (3 B):
//   mov rdi,r12   ; je @ `0x532be6` merge / post-call restore of cell cursor
//
// After E605 call @ `0x532beb`. NOT modelled: E607 call @ `0x532bf3` / E608 mov @ `0x532bf8` / E609 lea @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRdiR12_532bf0_U1Rva = 0x532bf0;
inline constexpr std::size_t kInit531d60PostBaseMovRdiR12_532bf0_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovRdiR12_532bf0_U1EndRva = 0x532bf3;

struct Init531d60PostBaseMovRdiR12_532bf0_U1Cells {
    const void* r12_in = nullptr;
    void** rdi_out = nullptr;
};

struct Init531d60PostBaseMovRdiR12_532bf0_U1Result {
    bool ok = false;
    int mov_rdi_r12_532bf0_count = 0;
    void* rdi = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRdiR12_532bf0_U1Result init_531d60_post_base_mov_rdi_r12_532bf0_u1(
    const Init531d60PostBaseMovRdiR12_532bf0_U1Cells& cells = {}) noexcept;

}  // namespace sotc
