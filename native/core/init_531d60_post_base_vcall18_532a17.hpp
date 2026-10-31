// Init `0x531d60` continue E first virtual-call +0x18 OBS `0x532a0c..0x532a1a` (14 B):
//   mov rax, [rsp+0x560]     ; vtable from prior stack prep (NOT modelled)
//   mov rdi, r14
//   call [rax+0x18]          ; slot +0x18 → 0x538200 (bare ret, elided)
//   // stop before mov rdi,[r13] @ 0x532a1a
//
// Always-on after E520 first `0xa54920` call-site (u1). Prefer this virtual-call
// peel over `0xa54920` body / descriptor stack-prep tourism and over deepen
// `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
// `0x6a5370`.
//
// Claimed: first save-data-ctor virtual-call control flow pin only (slot +0x18).
// NOT modelled: stack/vtable producers; callee body; shrunk `continue_e` tail.
// G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseVcall18532a17Rva = 0x532a0c;
inline constexpr std::size_t kInit531d60PostBaseVcall18532a17Size = 14;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18532a17EndRva = 0x532a1a;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18532a17SiteRva = 0x532a17;
inline constexpr std::uint32_t kInit531d60PostBaseVcall18532a17SlotOff = 0x18;
inline constexpr std::uint32_t kInit531d60PostBaseVcall18532a17StackOff = 0x560;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18532a17ResolvedTargetRva = 0x538200;

struct Init531d60PostBaseVcall18532a17Hooks {
    // Models call [rax+0x18](rdi) (callee body NOT lifted). Null → no-op.
    void (*call_vslot18)() = nullptr;
};

struct Init531d60PostBaseVcall18532a17Result {
    bool ok = false;
    int call_vslot18_count = 0;
};

[[nodiscard]] Init531d60PostBaseVcall18532a17Result init_531d60_post_base_vcall18_532a17(
    const Init531d60PostBaseVcall18532a17Hooks& hooks = {}) noexcept;

}  // namespace sotc
