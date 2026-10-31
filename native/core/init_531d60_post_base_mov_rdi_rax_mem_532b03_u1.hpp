// Init `0x531d60` continue E mov rdi,[rax] OBS `0x532b03..0x532b06` (3 B):
//   mov rdi,[rax]   ; rdi = *(0x2eff450) after lea rax,[0x2eff450]
//
// After E544 `mov_edx_2_532afe_u1` end @ `0x532b03`. NOT modelled: following
// tail through `0x532b0c` (continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRdiRaxMem_532b03_U1Rva = 0x532b03;
inline constexpr std::size_t kInit531d60PostBaseMovRdiRaxMem_532b03_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovRdiRaxMem_532b03_U1EndRva = 0x532b06;

struct Init531d60PostBaseMovRdiRaxMem_532b03_U1Cells {
    // Optional load source (address in rax). Null => counted, rdi=null.
    void* const* cell_ptr = nullptr;
};

struct Init531d60PostBaseMovRdiRaxMem_532b03_U1Result {
    bool ok = false;
    int mov_rdi_rax_mem_count = 0;
    void* rdi = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRdiRaxMem_532b03_U1Result
init_531d60_post_base_mov_rdi_rax_mem_532b03_u1(
    const Init531d60PostBaseMovRdiRaxMem_532b03_U1Cells& cells = {}) noexcept;

}  // namespace sotc
