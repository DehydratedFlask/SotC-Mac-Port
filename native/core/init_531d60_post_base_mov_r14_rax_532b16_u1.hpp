// Init `0x531d60` registry mov r14,rax OBS `0x532b16..0x532b19` (3 B):
//   mov r14,rax   ; save alloc result from call 0x17070 @ 0x532b11
//
// After E549 call @ `0x532b11`. NOT modelled: call 0x54dc10 @ `0x532b19`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR14Rax_532b16_U1Rva = 0x532b16;
inline constexpr std::size_t kInit531d60PostBaseMovR14Rax_532b16_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovR14Rax_532b16_U1EndRva = 0x532b19;

struct Init531d60PostBaseMovR14Rax_532b16_U1Cells {
    // Host stand-in for rax (0x17070 return). Null → counted, no r14 write.
    const void* rax_in = nullptr;
    void** r14_out = nullptr;
};

struct Init531d60PostBaseMovR14Rax_532b16_U1Result {
    bool ok = false;
    int mov_r14_rax_count = 0;
    void* r14 = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR14Rax_532b16_U1Result init_531d60_post_base_mov_r14_rax_532b16_u1(
    const Init531d60PostBaseMovR14Rax_532b16_U1Cells& cells = {}) noexcept;

}  // namespace sotc
