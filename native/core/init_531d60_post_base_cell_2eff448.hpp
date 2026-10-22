// Init `0x531d60` continue cell-install OBS `0x5323be..0x5323db` (29 B):
//   lea r14, [0x2eff448]
//   mov rdi, [r14]
//   cmp rdi, rbx / jz skip
//   test rdi, rdi / jz store
//   mov rax, [rdi]; call [rax+0x70]   ; destroy previous (hook)
//   mov [r14], rbx                   ; install
//   // stop before mov rax,[r12] @ 0x5323db
//
// Always-on after E484 first 0x53fd10 call-site. Prefer this lea/cell-install
// continue over `0x53fd10` / `0x17070` / `0x544fe0` / vcall-callee /
// stack-prep tourism, over trampoline/`0x52eb50` / `0x26800`/`0xf590` /
// staffroll bodies, and over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` /
// `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.
//
// Claimed: cell pin 0x2eff448 + replace/install control flow.
// NOT modelled: vcall+0x70 callee body; following vcall [rax+0x38].
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCell2eff448Rva = 0x5323be;
inline constexpr std::size_t kInit531d60PostBaseCell2eff448Size = 29;
inline constexpr std::uint64_t kInit531d60PostBaseCell2eff448EndRva = 0x5323db;
inline constexpr std::uint64_t kInit531d60PostBaseCell2eff448CellRva = 0x2eff448;

struct Init531d60PostBaseCell2eff448Cells {
    // Models rbx (object after alloc 0x490 + call 0x53fd10). Required.
    void* neu = nullptr;
    // Host stand-in for qword cell @ 0x2eff448. Required.
    void** cell = nullptr;
};

struct Init531d60PostBaseCell2eff448Hooks {
    // Models call [rax+0x70] on previous cell occupant. Null → skip body
    // (still counts as destroy path when prev non-null and != neu).
    void (*dtor_vslot70)(void* prev) = nullptr;
};

struct Init531d60PostBaseCell2eff448Result {
    bool ok = false;
    bool reused_same = false;
    bool destroyed_prev = false;
    bool installed = false;
    int dtor_vslot70_count = 0;
};

// Models OBS 0x5323be..0x5323db on measured Init success path.
[[nodiscard]] Init531d60PostBaseCell2eff448Result init_531d60_post_base_cell_2eff448(
    const Init531d60PostBaseCell2eff448Cells& cells = {},
    const Init531d60PostBaseCell2eff448Hooks& hooks = {}) noexcept;

}  // namespace sotc
