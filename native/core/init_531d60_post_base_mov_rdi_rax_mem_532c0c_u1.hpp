// Init `0x531d60` registry mov rdi,[rax] OBS `0x532c0c..0x532c0f` (3 B):
//   mov rdi,[rax]   ; load through rax after E611 mov edx @ `0x532c07`
//
// After E611 mov edx @ `0x532c07`. NOT modelled: tail @ `0x532c0f`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRdiRaxMem_532c0c_U1Rva = 0x532c0c;
inline constexpr std::size_t kInit531d60PostBaseMovRdiRaxMem_532c0c_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovRdiRaxMem_532c0c_U1EndRva = 0x532c0f;

struct Init531d60PostBaseMovRdiRaxMem_532c0c_U1Cells {
    void* const* cell_ptr = nullptr;
};

struct Init531d60PostBaseMovRdiRaxMem_532c0c_U1Result {
    bool ok = false;
    int mov_rdi_rax_mem_532c0c_count = 0;
    void* rdi = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRdiRaxMem_532c0c_U1Result
init_531d60_post_base_mov_rdi_rax_mem_532c0c_u1(
    const Init531d60PostBaseMovRdiRaxMem_532c0c_U1Cells& cells = {}) noexcept;

}  // namespace sotc
