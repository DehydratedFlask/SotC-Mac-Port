// Init `0x531d60` continue B call-site OBS `0x5327a0..0x5327a5` (5 B):
//   call 0x4c4950   ; first call after 1e4c0_u1 end @ 0x532793
//
// Always-on on the B fall-through path. Prep `0x532793..0x5327a0` (13 B) NOT
// modelled (mov ebx,eax .. movzx edi,r12b). NOT modelled: 0x4c4950 body beyond hook.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase4c4950U1Rva = 0x5327a0;
inline constexpr std::size_t kInit531d60PostBase4c4950U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase4c4950U1EndRva = 0x5327a5;
inline constexpr std::uint64_t kInit531d60PostBase4c4950U1TargetRva = 0x4c4950;
inline constexpr std::uint64_t kInit531d60PostBase4c4950U1PrepRva = 0x532793;
inline constexpr std::size_t kInit531d60PostBase4c4950U1PrepSize = 13;

struct Init531d60PostBase4c4950U1Hooks {
    int incoming_eax_from_1e4c0_u1 = 0;
    bool incoming_eax_known = false;
    void (*call_4c4950)(std::uint8_t edi) = nullptr;
};

struct Init531d60PostBase4c4950U1Result {
    bool ok = false;
    int call_4c4950_count = 0;
    std::uint8_t edi_arg = 0;
    bool edi_known = false;
};

[[nodiscard]] Init531d60PostBase4c4950U1Result init_531d60_post_base_4c4950_u1(
    const Init531d60PostBase4c4950U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
