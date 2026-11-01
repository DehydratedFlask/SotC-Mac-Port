// Init `0x531d60` registry mov [rbx],r14 OBS `0x532bf8..0x532bfb` (3 B):
//   mov [rbx],r14   ; store new object into global cell @ 0x2efd160 (null-r12 epilogue)
//
// After E607 call @ `0x532bf3`. NOT modelled: E609 lea rax @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRbx_532bf8_U1Rva = 0x532bf8;
inline constexpr std::size_t kInit531d60PostBaseMovR14MemRbx_532bf8_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRbx_532bf8_U1EndRva = 0x532bfb;

struct Init531d60PostBaseMovR14MemRbx_532bf8_U1Cells {
    void* r14_in = nullptr;
    void** cell_word = nullptr;
};

struct Init531d60PostBaseMovR14MemRbx_532bf8_U1Result {
    bool ok = false;
    int mov_r14_mem_rbx_532bf8_count = 0;
    void* stored = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR14MemRbx_532bf8_U1Result init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1(
    const Init531d60PostBaseMovR14MemRbx_532bf8_U1Cells& cells = {}) noexcept;

}  // namespace sotc
