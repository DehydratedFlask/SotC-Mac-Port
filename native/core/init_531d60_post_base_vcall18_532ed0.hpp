// Init `0x531d60` continue OBS `0x5322fd..0x53230b` (14 B): first 0x532ed0-series
// virtual call +0x18
//   mov rax, [rsp+0xa80]     ; vtable/object from prior stack prep (NOT modelled)
//   mov rdi, r14
//   call [rax+0x18]
//   // stop before next unit mov rdi,[rbx] @ 0x53230b
//
// Always-on after E476 first `0x532ed0` call-site. Prefer this virtual-call
// continue over `0x532ed0` body / stack-prep tourism, over trampoline/
// `0x52eb50` / `0x26800`/`0xf590` / staffroll / vcall-callee / `0x532ca0`
// bodies, and over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` /
// `0x140d90` / `0x14c2f0` / `0x6a5370`.
//
// Claimed: first 0x532ed0-series virtual-call control flow pin only (slot +0x18).
// NOT modelled: stack/vtable producers; callee body; later 0x532ed0 units.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseVcall18532ed0Rva = 0x5322fd;
inline constexpr std::size_t kInit531d60PostBaseVcall18532ed0Size = 14;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18532ed0EndRva = 0x53230b;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18532ed0SiteRva = 0x532308;
inline constexpr std::uint32_t kInit531d60PostBaseVcall18532ed0SlotOff = 0x18;
inline constexpr std::uint32_t kInit531d60PostBaseVcall18532ed0StackOff = 0xa80;

struct Init531d60PostBaseVcall18532ed0Hooks {
    // Models call [rax+0x18](rdi) (callee body NOT lifted). Null → no-op.
    void (*call_vslot18)() = nullptr;
};

struct Init531d60PostBaseVcall18532ed0Result {
    bool ok = false;
    int call_vslot18_count = 0;
};

// Models OBS 0x5322fd..0x53230b on measured Init success path.
[[nodiscard]] Init531d60PostBaseVcall18532ed0Result init_531d60_post_base_vcall18_532ed0(
    const Init531d60PostBaseVcall18532ed0Hooks& hooks = {}) noexcept;

}  // namespace sotc
