// Init `0x531d60` registry mov r13,[r12+0x40] OBS `0x532bae..0x532bb3` (5 B):
//   mov r13,[r12+0x40]   ; fall-through after jne @ `0x532bac`
//
// After E589 jne @ `0x532bac`. NOT modelled: lea @ `0x532bb3..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR13MemR12_40_532bae_U1Rva = 0x532bae;
inline constexpr std::size_t kInit531d60PostBaseMovR13MemR12_40_532bae_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovR13MemR12_40_532bae_U1EndRva = 0x532bb3;
inline constexpr std::uint32_t kInit531d60PostBaseMovR13MemR12_40_532bae_U1Disp = 0x40;

struct Init531d60PostBaseMovR13MemR12_40_532bae_U1Cells {
    const void* r12_base = nullptr;
    void** r13_out = nullptr;
};

struct Init531d60PostBaseMovR13MemR12_40_532bae_U1Result {
    bool ok = false;
    int mov_r13_mem_r12_40_532bae_count = 0;
    void* r13 = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR13MemR12_40_532bae_U1Result init_531d60_post_base_mov_r13_mem_r12_40_532bae_u1(
    const Init531d60PostBaseMovR13MemR12_40_532bae_U1Cells& cells = {}) noexcept;

}  // namespace sotc
