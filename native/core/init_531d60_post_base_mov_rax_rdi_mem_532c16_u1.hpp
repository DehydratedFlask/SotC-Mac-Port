// Init `0x531d60` registry mov rax,[rdi] OBS `0x532c16..0x532c19` (3 B):
//   48 8b 07   ; rax = vtable pointer of the object loaded at 0x532c0c
//
// After E613 lea rsi,[r14+0x3ec0] @ `0x532c0f`. NOT modelled: E615 call [rax+0x70] @ `0x532c19`.
// G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRaxRdiMem_532c16_U1Rva = 0x532c16;
inline constexpr std::size_t kInit531d60PostBaseMovRaxRdiMem_532c16_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovRaxRdiMem_532c16_U1EndRva = 0x532c19;

struct Init531d60PostBaseMovRaxRdiMem_532c16_U1Cells {
    // Host stand-in for the qword at [rdi] (vtable pointer). Null => rax = null.
    void* const* cell_ptr = nullptr;
    void** rax_out = nullptr;
};

struct Init531d60PostBaseMovRaxRdiMem_532c16_U1Result {
    bool ok = false;
    int mov_rax_rdi_mem_532c16_count = 0;
    void* rax = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRaxRdiMem_532c16_U1Result
init_531d60_post_base_mov_rax_rdi_mem_532c16_u1(
    const Init531d60PostBaseMovRaxRdiMem_532c16_U1Cells& cells = {}) noexcept;

}  // namespace sotc
