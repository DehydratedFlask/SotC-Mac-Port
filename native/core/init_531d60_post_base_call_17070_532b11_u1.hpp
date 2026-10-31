// Init `0x531d60` registry call 0x17070 OBS `0x532b11..0x532b16` (5 B):
//   call 0x17070   ; size 0x98 in edi from prior mov edi peel (E548)
//   // stop before mov r14,rax @ 0x532b16
//
// After E548 mov edi @ `0x532b0c`. NOT modelled: 0x17070 body; following mov r14,rax.
// G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCall17070_532b11_U1Rva = 0x532b11;
inline constexpr std::size_t kInit531d60PostBaseCall17070_532b11_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseCall17070_532b11_U1EndRva = 0x532b16;
inline constexpr std::uint64_t kInit531d60PostBaseCall17070_532b11_U1TargetRva = 0x17070;

struct Init531d60PostBaseCall17070_532b11_U1Hooks {
    void (*call_17070)() = nullptr;
};

struct Init531d60PostBaseCall17070_532b11_U1Result {
    bool ok = false;
    int call_17070_count = 0;
};

[[nodiscard]] Init531d60PostBaseCall17070_532b11_U1Result init_531d60_post_base_call_17070_532b11_u1(
    const Init531d60PostBaseCall17070_532b11_U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
