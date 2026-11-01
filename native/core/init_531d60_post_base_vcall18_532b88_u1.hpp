// Init `0x531d60` registry virtual-call +0x18 OBS `0x532b88..0x532b8b` (3 B):
//   call [rax+0x18]   ; fall-through after lea rdi,[rbx+0x28] @ `0x532b84`
//
// After E577 lea rdi @ `0x532b84`. Prep mov rax,[rbx+0x48] @ `0x532b80` NOT modelled here.
// NOT modelled: callee body; add rbx,+0x58 tail @ `0x532b8b..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseVcall18_532b88_U1Rva = 0x532b88;
inline constexpr std::size_t kInit531d60PostBaseVcall18_532b88_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18_532b88_U1EndRva = 0x532b8b;
inline constexpr std::uint32_t kInit531d60PostBaseVcall18_532b88_U1SlotOff = 0x18;

struct Init531d60PostBaseVcall18_532b88_U1Hooks {
    void (*call_vslot18)() = nullptr;
};

struct Init531d60PostBaseVcall18_532b88_U1Result {
    bool ok = false;
    int call_vslot18_count = 0;
};

[[nodiscard]] Init531d60PostBaseVcall18_532b88_U1Result init_531d60_post_base_vcall18_532b88_u1(
    const Init531d60PostBaseVcall18_532b88_U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
