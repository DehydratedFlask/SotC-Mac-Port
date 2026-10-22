// Init `0x531d60` continue OBS `0x532369..0x532377` (14 B): second 0x532ed0-series
// virtual call +0x18
//   mov rax, [rsp+0xa30]     ; vtable/object from prior stack prep (NOT modelled)
//   mov rdi, r14
//   call [rax+0x18]
//   // stop before mov edi,0x460 / call 0x17070 @ 0x532377
//
// Always-on after E478 second `0x532ed0` call-site. Prefer this virtual-call
// continue over `0x532ed0` body / stack-prep tourism, over trampoline/
// `0x52eb50` / `0x26800`/`0xf590` / staffroll / vcall-callee / `0x532ca0`
// bodies, and over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` /
// `0x140d90` / `0x14c2f0` / `0x6a5370`.
//
// Claimed: second 0x532ed0-series virtual-call control flow pin only (slot +0x18).
// NOT modelled: stack/vtable producers; callee body; later helpers
// (next call after this end is 0x17070, claimed separately as E480).
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseVcall18532ed0U2Rva = 0x532369;
inline constexpr std::size_t kInit531d60PostBaseVcall18532ed0U2Size = 14;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18532ed0U2EndRva = 0x532377;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18532ed0U2SiteRva = 0x532374;
inline constexpr std::uint32_t kInit531d60PostBaseVcall18532ed0U2SlotOff = 0x18;
inline constexpr std::uint32_t kInit531d60PostBaseVcall18532ed0U2StackOff = 0xa30;

struct Init531d60PostBaseVcall18532ed0U2Hooks {
    // Models call [rax+0x18](rdi) (callee body NOT lifted). Null → no-op.
    void (*call_vslot18)() = nullptr;
};

struct Init531d60PostBaseVcall18532ed0U2Result {
    bool ok = false;
    int call_vslot18_count = 0;
};

// Models OBS 0x532369..0x532377 on measured Init success path.
[[nodiscard]] Init531d60PostBaseVcall18532ed0U2Result init_531d60_post_base_vcall18_532ed0_u2(
    const Init531d60PostBaseVcall18532ed0U2Hooks& hooks = {}) noexcept;

}  // namespace sotc
