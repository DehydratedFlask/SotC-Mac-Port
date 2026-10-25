// Init `0x531d60` continue OBS `0x5323db..0x5323e5` (10 B): virtual call +0x38
//   mov rax, [r12]           ; vtable/object from prior r12 (NOT modelled)
//   mov rdi, r12
//   call [rax+0x38]
//   // stop before lea r13 @ 0x5323e5
//
// Always-on after E485 cell-install `0x2eff448`. Prefer this virtual-call
// continue over vcall-callee / `0x53fd10` / `0x17070` / stack-prep tourism,
// over trampoline/`0x52eb50` / `0x26800`/`0xf590` / staffroll bodies, and
// over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
// `0x14c2f0` / `0x6a5370`.
//
// Claimed: virtual-call control flow pin only (slot +0x38).
// NOT modelled: r12/vtable producers; callee body; following lea r13
// (claimed separately as E487). PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseVcall38Rva = 0x5323db;
inline constexpr std::size_t kInit531d60PostBaseVcall38Size = 10;
inline constexpr std::uint64_t kInit531d60PostBaseVcall38EndRva = 0x5323e5;
inline constexpr std::uint64_t kInit531d60PostBaseVcall38SiteRva = 0x5323e2;
inline constexpr std::uint32_t kInit531d60PostBaseVcall38SlotOff = 0x38;

struct Init531d60PostBaseVcall38Hooks {
    // Models call [rax+0x38](rdi) (callee body NOT lifted). Null → no-op.
    void (*call_vslot38)() = nullptr;
};

struct Init531d60PostBaseVcall38Result {
    bool ok = false;
    int call_vslot38_count = 0;
};

// Models OBS 0x5323db..0x5323e5 on measured Init success path.
[[nodiscard]] Init531d60PostBaseVcall38Result init_531d60_post_base_vcall38(
    const Init531d60PostBaseVcall38Hooks& hooks = {}) noexcept;

}  // namespace sotc
