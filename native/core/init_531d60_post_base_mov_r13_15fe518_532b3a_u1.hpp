// Init `0x531d60` registry mov r13 OBS `0x532b3a..0x532b41` (7 B):
//   mov r13,[0x15fe518]   ; __stack_chk_guard pointer cell
//
// After E558 lea rbx @ `0x532b33`. NOT modelled: mov r12,[rbx] @ `0x532b41` (E560 peel) / compare-store @ `0x532b44..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR13_15fe518_532b3a_U1Rva = 0x532b3a;
inline constexpr std::size_t kInit531d60PostBaseMovR13_15fe518_532b3a_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseMovR13_15fe518_532b3a_U1EndRva = 0x532b41;
inline constexpr std::uint64_t kInit531d60PostBaseMovR13_15fe518_532b3a_U1GuardCellRva = 0x15fe518;

struct Init531d60PostBaseMovR13_15fe518_532b3a_U1Cells {
    // Host stand-in for the qword at 0x15fe518 (pointer to guard value). Null → native default.
    void** guard_cell_slot = nullptr;
    void** r13_out = nullptr;
};

struct Init531d60PostBaseMovR13_15fe518_532b3a_U1Result {
    bool ok = false;
    int mov_r13_rip_count = 0;
    void* r13 = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR13_15fe518_532b3a_U1Result
init_531d60_post_base_mov_r13_15fe518_532b3a_u1(
    const Init531d60PostBaseMovR13_15fe518_532b3a_U1Cells& cells = {}) noexcept;

}  // namespace sotc
