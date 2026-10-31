// Init `0x531d60` registry mov rbx,rax OBS `0x532b1e..0x532b21` (3 B):
//   mov rbx,rax   ; save table base from call 0x54dc10 @ 0x532b19
//
// After E551 call @ `0x532b19`. NOT modelled: call 0x54dc20 @ `0x532b21`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRbxRax_532b1e_U1Rva = 0x532b1e;
inline constexpr std::size_t kInit531d60PostBaseMovRbxRax_532b1e_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovRbxRax_532b1e_U1EndRva = 0x532b21;

struct Init531d60PostBaseMovRbxRax_532b1e_U1Cells {
    // Host stand-in for rax (0x54dc10 return). Null → counted, no rbx write.
    const void* rax_in = nullptr;
    void** rbx_out = nullptr;
};

struct Init531d60PostBaseMovRbxRax_532b1e_U1Result {
    bool ok = false;
    int mov_rbx_rax_count = 0;
    void* rbx = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRbxRax_532b1e_U1Result init_531d60_post_base_mov_rbx_rax_532b1e_u1(
    const Init531d60PostBaseMovRbxRax_532b1e_U1Cells& cells = {}) noexcept;

}  // namespace sotc
