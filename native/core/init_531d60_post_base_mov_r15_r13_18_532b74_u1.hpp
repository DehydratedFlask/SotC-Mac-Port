// Init `0x531d60` registry mov r15,[r13+0x18] OBS `0x532b74..0x532b78` (4 B):
//   mov r15,[r13+0x18]   ; fall-through after mov rbx @ `0x532b70`
//
// After E571 mov rbx @ `0x532b70`. NOT modelled: cmp tail @ `0x532b78..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR15R13_18_532b74_U1Rva = 0x532b74;
inline constexpr std::size_t kInit531d60PostBaseMovR15R13_18_532b74_U1Size = 4;
inline constexpr std::uint64_t kInit531d60PostBaseMovR15R13_18_532b74_U1EndRva = 0x532b78;
inline constexpr std::uint32_t kInit531d60PostBaseMovR15R13_18_532b74_U1Disp = 0x18;

struct Init531d60PostBaseMovR15R13_18_532b74_U1Cells {
    const void* r13_base = nullptr;
    void** r15_out = nullptr;
};

struct Init531d60PostBaseMovR15R13_18_532b74_U1Result {
    bool ok = false;
    int mov_r15_r13_18_count = 0;
    void* r15 = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR15R13_18_532b74_U1Result init_531d60_post_base_mov_r15_r13_18_532b74_u1(
    const Init531d60PostBaseMovR15R13_18_532b74_U1Cells& cells = {}) noexcept;

}  // namespace sotc
