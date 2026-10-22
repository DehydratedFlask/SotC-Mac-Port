// Init `0x531d60` continue OBS `0x5320b1..0x5320bf` (14 B): virtual call +0x18
//   mov rax, [rsp+0xc10]     ; vtable/object from prior stack prep (NOT modelled)
//   mov rdi, r14
//   call [rax+0x18]
//   // stop before next unit mov rdi,[rbx] @ 0x5320bf
//
// Always-on after E466 `0x532ca0` call-site. Prefer this virtual-call continue
// over `0x532ca0` body / stack-prep tourism, over trampoline/`0x52eb50` /
// `0x26800`/`0xf590` / staffroll bodies, and over deepen `0x1290` /
// `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
// `0x6a5370`.
//
// Claimed: virtual-call control flow pin only (slot +0x18).
// NOT modelled: stack/vtable producers; callee body; later 0x532ca0 units.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseVcall18Rva = 0x5320b1;
inline constexpr std::size_t kInit531d60PostBaseVcall18Size = 14;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18EndRva = 0x5320bf;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18SiteRva = 0x5320bc;
inline constexpr std::uint32_t kInit531d60PostBaseVcall18SlotOff = 0x18;

struct Init531d60PostBaseVcall18Hooks {
    // Models call [rax+0x18](rdi) (callee body NOT lifted). Null → no-op.
    void (*call_vslot18)() = nullptr;
};

struct Init531d60PostBaseVcall18Result {
    bool ok = false;
    int call_vslot18_count = 0;
};

// Models OBS 0x5320b1..0x5320bf on measured Init success path.
[[nodiscard]] Init531d60PostBaseVcall18Result init_531d60_post_base_vcall18(
    const Init531d60PostBaseVcall18Hooks& hooks = {}) noexcept;

}  // namespace sotc
