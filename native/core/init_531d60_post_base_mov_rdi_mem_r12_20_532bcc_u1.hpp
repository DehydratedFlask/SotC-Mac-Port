// Init `0x531d60` registry mov rdi,[r12+0x20] OBS `0x532bcc..0x532bd1` (5 B):
//   mov rdi,[r12+0x20]   ; fall-through after call 0x170c0 @ `0x532bc7`
//
// After E596 call @ `0x532bc7`. NOT modelled: E598 test @ `0x532bd1` / tail @ `0x532bd4..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRdiMemR12_20_532bcc_U1Rva = 0x532bcc;
inline constexpr std::size_t kInit531d60PostBaseMovRdiMemR12_20_532bcc_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovRdiMemR12_20_532bcc_U1EndRva = 0x532bd1;
inline constexpr std::uint32_t kInit531d60PostBaseMovRdiMemR12_20_532bcc_U1Disp = 0x20;

struct Init531d60PostBaseMovRdiMemR12_20_532bcc_U1Cells {
    const void* r12_base = nullptr;
    void** rdi_out = nullptr;
};

struct Init531d60PostBaseMovRdiMemR12_20_532bcc_U1Result {
    bool ok = false;
    int mov_rdi_mem_r12_20_532bcc_count = 0;
    void* rdi = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRdiMemR12_20_532bcc_U1Result init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1(
    const Init531d60PostBaseMovRdiMemR12_20_532bcc_U1Cells& cells = {}) noexcept;

}  // namespace sotc
