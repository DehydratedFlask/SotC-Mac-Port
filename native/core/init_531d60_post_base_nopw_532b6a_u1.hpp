// Init `0x531d60` registry nopw padding OBS `0x532b6a..0x532b70` (6 B):
//   66 0f 1f 44 00 00   ; nopw (%rax,%rax) alignment padding before the 0x30-stride loop
//
// Byte-pinned inert unit: the pinned ELF carries a 6-byte NOP here, not an instruction with
// architectural effect. E570 previously modelled a `lea rsi,[rsp+0x10]` at this RVA; that claim
// was withdrawn in E613 after re-reading the pinned bytes, and the checker now hashes the raw
// region (no peel substitutions).
//
// After E569 je @ `0x532b68`; next unit E571 mov rbx,[r13+0x10] @ `0x532b70`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseNopw_532b6a_U1Rva = 0x532b6a;
inline constexpr std::size_t kInit531d60PostBaseNopw_532b6a_U1Size = 6;
inline constexpr std::uint64_t kInit531d60PostBaseNopw_532b6a_U1EndRva = 0x532b70;

struct Init531d60PostBaseNopw_532b6a_U1Result {
    bool ok = false;
    int nopw_532b6a_count = 0;
};

// Inert: counts execution only; the padding changes no register or memory state.
[[nodiscard]] Init531d60PostBaseNopw_532b6a_U1Result
init_531d60_post_base_nopw_532b6a_u1() noexcept;

}  // namespace sotc
