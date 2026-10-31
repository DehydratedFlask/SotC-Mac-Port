// Init `0x531d60` continue E mov rax,[rdi] OBS `0x532b06..0x532b09` (3 B):
//   mov rax,[rdi]   ; rax = vtable pointer from object in rdi
//
// After E545 `mov_rdi_rax_mem_532b03_u1` end @ `0x532b06`. NOT modelled: following
// tail through `0x532b0c` (continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRaxRdiMem_532b06_U1Rva = 0x532b06;
inline constexpr std::size_t kInit531d60PostBaseMovRaxRdiMem_532b06_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovRaxRdiMem_532b06_U1EndRva = 0x532b09;

struct Init531d60PostBaseMovRaxRdiMem_532b06_U1Cells {
    // Optional load source (address in rdi). Null => counted, rax=null.
    void* const* cell_ptr = nullptr;
};

struct Init531d60PostBaseMovRaxRdiMem_532b06_U1Result {
    bool ok = false;
    int mov_rax_rdi_mem_count = 0;
    void* rax = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRaxRdiMem_532b06_U1Result
init_531d60_post_base_mov_rax_rdi_mem_532b06_u1(
    const Init531d60PostBaseMovRaxRdiMem_532b06_U1Cells& cells = {}) noexcept;

}  // namespace sotc
