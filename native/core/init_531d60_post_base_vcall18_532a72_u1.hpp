// Init `0x531d60` continue E second virtual-call +0x18 OBS `0x532a72..0x532a75` (3 B):
//   call [rax+0x18]   ; slot +0x18 → 0x538200 (bare ret, elided)
//
// After E522 `a54920_532a62_u1` end @ `0x532a67`. Prep `0x532a67..0x532a72` (11 B:
// mov rax,[rsp+0x500]; mov rdi,r14) NOT modelled as pinned units in `continue_e`.
// NOT modelled: stack/vtable producers; callee body. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseVcall18_532a72_U1Rva = 0x532a72;
inline constexpr std::size_t kInit531d60PostBaseVcall18_532a72_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18_532a72_U1EndRva = 0x532a75;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18_532a72_U1PrepRva = 0x532a67;
inline constexpr std::size_t kInit531d60PostBaseVcall18_532a72_U1PrepSize = 11;
inline constexpr std::uint32_t kInit531d60PostBaseVcall18_532a72_U1StackOff = 0x500;
inline constexpr std::uint32_t kInit531d60PostBaseVcall18_532a72_U1SlotOff = 0x18;
inline constexpr std::uint64_t kInit531d60PostBaseVcall18_532a72_U1ResolvedTargetRva = 0x538200;

struct Init531d60PostBaseVcall18_532a72_U1Hooks {
    void (*call_vslot18)() = nullptr;
};

struct Init531d60PostBaseVcall18_532a72_U1Result {
    bool ok = false;
    int call_vslot18_count = 0;
};

[[nodiscard]] Init531d60PostBaseVcall18_532a72_U1Result init_531d60_post_base_vcall18_532a72_u1(
    const Init531d60PostBaseVcall18_532a72_U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
