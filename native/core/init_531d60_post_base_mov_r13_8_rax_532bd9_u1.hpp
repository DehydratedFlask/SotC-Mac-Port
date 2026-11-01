// Init `0x531d60` registry mov [r13+0x8],rax OBS `0x532bd9..0x532bde` (5 B):
//   mov [r13+0x8],rax   ; store vtable pointer into cursor node
//
// After E600 `mov_rax_rdi_mem_532bd6_u1` end @ `0x532bd9`. NOT modelled: tail @
// `0x532bde`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR13_8_Rax_532bd9_U1Rva = 0x532bd9;
inline constexpr std::size_t kInit531d60PostBaseMovR13_8_Rax_532bd9_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovR13_8_Rax_532bd9_U1EndRva = 0x532bde;
inline constexpr std::uint32_t kInit531d60PostBaseMovR13_8_Rax_532bd9_U1Disp = 0x8;

struct Init531d60PostBaseMovR13_8_Rax_532bd9_U1Cells {
    void* r13_base = nullptr;
    void* rax_in = nullptr;
    void** r13_8_out = nullptr;
};

struct Init531d60PostBaseMovR13_8_Rax_532bd9_U1Result {
    bool ok = false;
    int mov_r13_8_rax_532bd9_count = 0;
    void* rax_stored = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR13_8_Rax_532bd9_U1Result
init_531d60_post_base_mov_r13_8_rax_532bd9_u1(
    const Init531d60PostBaseMovR13_8_Rax_532bd9_U1Cells& cells = {}) noexcept;

}  // namespace sotc
