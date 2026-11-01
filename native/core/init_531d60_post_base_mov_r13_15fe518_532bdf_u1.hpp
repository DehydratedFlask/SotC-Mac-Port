// Init `0x531d60` registry mov r13 OBS `0x532bdf..0x532be6` (7 B):
//   4c 8b 2d 32 b9 0c 01   ; mov r13,[0x15fe518] — __stack_chk_guard pointer cell
//
// After E613 mov rdi,[r12] @ `0x532bdb`. E599-E602 previously decoded the tail of this
// RIP-relative load as `sub eax,0x010cb932` at `0x532be1`; E613 restores the real boundary.
//
// NOT modelled: E604 test/je @ `0x532be6` and the delete tail. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR13_15fe518_532bdf_U1Rva = 0x532bdf;
inline constexpr std::size_t kInit531d60PostBaseMovR13_15fe518_532bdf_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseMovR13_15fe518_532bdf_U1EndRva = 0x532be6;
inline constexpr std::uint64_t kInit531d60PostBaseMovR13_15fe518_532bdf_U1GuardCellRva = 0x15fe518;

struct Init531d60PostBaseMovR13_15fe518_532bdf_U1Cells {
    // Host stand-in for the qword at 0x15fe518 (pointer to guard value). Null -> native default.
    void** guard_cell_slot = nullptr;
    void** r13_out = nullptr;
};

struct Init531d60PostBaseMovR13_15fe518_532bdf_U1Result {
    bool ok = false;
    int mov_r13_rip_532bdf_count = 0;
    void* r13 = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR13_15fe518_532bdf_U1Result
init_531d60_post_base_mov_r13_15fe518_532bdf_u1(
    const Init531d60PostBaseMovR13_15fe518_532bdf_U1Cells& cells = {}) noexcept;

}  // namespace sotc
