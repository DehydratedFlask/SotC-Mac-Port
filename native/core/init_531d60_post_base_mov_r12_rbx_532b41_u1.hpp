// Init `0x531d60` registry mov r12,[rbx] OBS `0x532b41..0x532b44` (3 B):
//   mov r12,[rbx]   ; previous registry object pointer from global cell @ 0x2efd160
//
// After E559 mov r13 @ `0x532b3a`. NOT modelled: cmp r12,r14 @ `0x532b44` (E561 peel) / destroy-store @ `0x532b47..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR12Rbx_532b41_U1Rva = 0x532b41;
inline constexpr std::size_t kInit531d60PostBaseMovR12Rbx_532b41_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseMovR12Rbx_532b41_U1EndRva = 0x532b44;

struct Init531d60PostBaseMovR12Rbx_532b41_U1Cells {
    // Host stand-in for the qword at [rbx] (lea @ 0x532b33). Null → native registry cell load.
    void** cell_word = nullptr;
    void** r12_out = nullptr;
};

struct Init531d60PostBaseMovR12Rbx_532b41_U1Result {
    bool ok = false;
    int mov_r12_rbx_count = 0;
    void* r12 = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR12Rbx_532b41_U1Result init_531d60_post_base_mov_r12_rbx_532b41_u1(
    const Init531d60PostBaseMovR12Rbx_532b41_U1Cells& cells = {}) noexcept;

}  // namespace sotc
