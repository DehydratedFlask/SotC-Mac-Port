// Init `0x531d60` registry lea rsi OBS `0x532c0f..0x532c16` (7 B):
//   49 8d b6 c0 3e 00 00   ; lea rsi,[r14+0x3ec0] — address of the 0x3ec0 vector header
//
// `r14` here is the caller frame value restored by E610 mov r14,[rsp+0x20] @ `0x532c02`.
// After E612 mov rdi,[rax] @ `0x532c0c`. NOT modelled: E614 mov rax,[rdi] @ `0x532c16`.
// G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRsiR14_3ec0_532c0f_U1Rva = 0x532c0f;
inline constexpr std::size_t kInit531d60PostBaseLeaRsiR14_3ec0_532c0f_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRsiR14_3ec0_532c0f_U1EndRva = 0x532c16;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRsiR14_3ec0_532c0f_U1Disp = 0x3ec0;

struct Init531d60PostBaseLeaRsiR14_3ec0_532c0f_U1Cells {
    // Host value of r14 (the caller frame pointer restored at 0x532c02).
    const void* r14_in = nullptr;
    void** rsi_out = nullptr;
};

struct Init531d60PostBaseLeaRsiR14_3ec0_532c0f_U1Result {
    bool ok = false;
    int lea_rsi_r14_3ec0_532c0f_count = 0;
    void* rsi = nullptr;
};

[[nodiscard]] Init531d60PostBaseLeaRsiR14_3ec0_532c0f_U1Result
init_531d60_post_base_lea_rsi_r14_3ec0_532c0f_u1(
    const Init531d60PostBaseLeaRsiR14_3ec0_532c0f_U1Cells& cells = {}) noexcept;

}  // namespace sotc
