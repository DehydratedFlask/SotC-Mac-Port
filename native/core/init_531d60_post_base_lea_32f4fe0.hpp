// Init `0x531d60` continue OBS `0x5323e5..0x5323ec` (7 B): lea r13 pin
//   lea r13, [0x32f4fe0]     ; global/cell address into r13
//   // stop before mov rdi,[r13] / call 0xa2caf0 @ 0x5323ec
//
// Always-on after E486 vcall [rax+0x38]. Prefer this lea pin continue
// over vcall+0x38 callee / `0x53fd10` / `0x17070` / stack-prep tourism,
// over trampoline/`0x52eb50` / `0x26800`/`0xf590` / staffroll bodies, and
// over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
// `0x14c2f0` / `0x6a5370`.
//
// Claimed: r13 cell-address pin for 0x32f4fe0.
// NOT modelled: mov rdi,[r13]; call 0xa2caf0 body; following stack prep.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLea32f4fe0Rva = 0x5323e5;
inline constexpr std::size_t kInit531d60PostBaseLea32f4fe0Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseLea32f4fe0EndRva = 0x5323ec;
inline constexpr std::uint64_t kInit531d60PostBaseLea32f4fe0CellRva = 0x32f4fe0;

// Host-backed cell: models lea r13 loading &0x32f4fe0.
struct Init531d60PostBaseLea32f4fe0Cells {
    // Host stand-in for the lea r13 target cell. Required with r13_out.
    void* r13_cell = nullptr;
    // Receives r13_cell (models lea). Required with r13_cell.
    void** r13_out = nullptr;
};

struct Init531d60PostBaseLea32f4fe0Result {
    bool ok = false;
    int lea_count = 0;
};

// Models OBS 0x5323e5..0x5323ec on measured Init success path.
[[nodiscard]] Init531d60PostBaseLea32f4fe0Result init_531d60_post_base_lea_32f4fe0(
    const Init531d60PostBaseLea32f4fe0Cells& cells = {}) noexcept;

}  // namespace sotc
