// Init `0x531d60` continue E third virtual-call +0x18 OBS `0x532af2..0x532af5` (3 B):
//   call [rax+0x18]   ; slot +0x18 → 0x5388c0 (bare ret, elided)
//
// After E540 `mov_rdi_rbx_532aef_u1` end @ `0x532af2`. Prep `0x532aef..0x532af2` (3 B:
// mov rdi,rbx) NOT modelled as pinned units in `continue_e`.
// NOT modelled: stack/vtable producers; callee body. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseVcall18_532af2_U1Rva = 0x532af2;
inline constexpr std::size_t kInit531d60PostBaseVcall18_532af2_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18_532af2_U1EndRva = 0x532af5;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18_532af2_U1PrepRva = 0x532aef;
inline constexpr std::size_t kInit531d60PostBaseVcall18_532af2_U1PrepSize = 3;
inline constexpr std::uint32_t kInit531d60PostBaseVcall18_532af2_U1SlotOff = 0x18;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18_532af2_U1ResolvedTargetRva = 0x5388c0;

struct Init531d60PostBaseVcall18_532af2_U1Hooks {
    void (*call_vslot18)() = nullptr;
};

struct Init531d60PostBaseVcall18_532af2_U1Result {
    bool ok = false;
    int call_vslot18_count = 0;
};

[[nodiscard]] Init531d60PostBaseVcall18_532af2_U1Result init_531d60_post_base_vcall18_532af2_u1(
    const Init531d60PostBaseVcall18_532af2_U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
