// Init `0x531d60` registry mov rax,[rdi] OBS `0x532bd6..0x532bd9` (3 B):
//   mov rax,[rdi]   ; rax = vtable pointer from object in rdi
//
// After E599 `je_532bdb_532bd4_u1` end @ `0x532bd6`. NOT modelled: E601
// `mov_r13_8_rax_532bd9_u1` @ `0x532bd9` / tail @ `0x532bde`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRaxRdiMem_532bd6_U1Rva = 0x532bd6;
inline constexpr std::size_t kInit531d60PostBaseMovRaxRdiMem_532bd6_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovRaxRdiMem_532bd6_U1EndRva = 0x532bd9;

struct Init531d60PostBaseMovRaxRdiMem_532bd6_U1Cells {
    // Optional load source (address in rdi). Null => counted, rax=null.
    void* const* cell_ptr = nullptr;
};

struct Init531d60PostBaseMovRaxRdiMem_532bd6_U1Result {
    bool ok = false;
    int mov_rax_rdi_mem_532bd6_count = 0;
    void* rax = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRaxRdiMem_532bd6_U1Result
init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1(
    const Init531d60PostBaseMovRaxRdiMem_532bd6_U1Cells& cells = {}) noexcept;

}  // namespace sotc
