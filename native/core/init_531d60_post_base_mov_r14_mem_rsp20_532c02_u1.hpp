// Init `0x531d60` registry mov r14,[rsp+0x20] OBS `0x532c02..0x532c07` (5 B):
//   mov r14,[rsp+0x20]   ; reload saved r14 after lea rax @ `0x532bfb`
//
// After E609 lea @ `0x532bfb`. NOT modelled: E611 mov edx @ `0x532c07` / tail @ `0x532c0c`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRsp20_532c02_U1Rva = 0x532c02;
inline constexpr std::size_t kInit531d60PostBaseMovR14MemRsp20_532c02_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRsp20_532c02_U1EndRva = 0x532c07;

struct Init531d60PostBaseMovR14MemRsp20_532c02_U1Cells {
    const void* const* rsp20_in = nullptr;
    void** r14_out = nullptr;
};

struct Init531d60PostBaseMovR14MemRsp20_532c02_U1Result {
    bool ok = false;
    int mov_r14_mem_rsp20_532c02_count = 0;
    void* r14_loaded = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR14MemRsp20_532c02_U1Result
init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1(
    const Init531d60PostBaseMovR14MemRsp20_532c02_U1Cells& cells = {}) noexcept;

}  // namespace sotc
