// Init `0x531d60` continue cell-install OBS `0x53238c..0x5323a9` (29 B):
//   lea r14, [0x2eff450]
//   mov rdi, [r14]
//   cmp rdi, rbx / jz skip
//   test rdi, rdi / jz store
//   mov rax, [rdi]; call [rax+0x70]   ; destroy previous (hook)
//   mov [r14], rbx                   ; install
//   // stop before mov edi,0x490 / call 0x17070 @ 0x5323a9
//
// Always-on after E481 first 0x544fe0 call-site. Prefer this lea/cell-install
// continue over `0x544fe0` / `0x17070` / `0x532ed0` / vcall-callee /
// stack-prep tourism, over trampoline/`0x52eb50` / `0x26800`/`0xf590` /
// staffroll bodies, and over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` /
// `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.
//
// Claimed: cell pin 0x2eff450 + replace/install control flow.
// NOT modelled: vcall+0x70 callee body; following second 0x17070
// (claimed separately as E483) / 0x53fd10. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCell2eff450Rva = 0x53238c;
inline constexpr std::size_t kInit531d60PostBaseCell2eff450Size = 29;
inline constexpr std::uint64_t kInit531d60PostBaseCell2eff450EndRva = 0x5323a9;
inline constexpr std::uint64_t kInit531d60PostBaseCell2eff450CellRva = 0x2eff450;

struct Init531d60PostBaseCell2eff450Cells {
    // Models rbx (object after alloc 0x460 + call 0x544fe0). Required.
    void* neu = nullptr;
    // Host stand-in for qword cell @ 0x2eff450. Required.
    void** cell = nullptr;
};

struct Init531d60PostBaseCell2eff450Hooks {
    // Models call [rax+0x70] on previous cell occupant. Null → skip body
    // (still counts as destroy path when prev non-null and != neu).
    void (*dtor_vslot70)(void* prev) = nullptr;
};

struct Init531d60PostBaseCell2eff450Result {
    bool ok = false;
    bool reused_same = false;
    bool destroyed_prev = false;
    bool installed = false;
    int dtor_vslot70_count = 0;
};

// Models OBS 0x53238c..0x5323a9 on measured Init success path.
[[nodiscard]] Init531d60PostBaseCell2eff450Result init_531d60_post_base_cell_2eff450(
    const Init531d60PostBaseCell2eff450Cells& cells = {},
    const Init531d60PostBaseCell2eff450Hooks& hooks = {}) noexcept;

}  // namespace sotc
