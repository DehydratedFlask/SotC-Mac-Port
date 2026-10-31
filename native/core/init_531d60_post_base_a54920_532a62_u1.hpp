// Init `0x531d60` continue E second call-site OBS `0x532a62..0x532a67` (5 B):
//   call 0xa54920   ; save-data ctor 2 ("save_%s_saves_used")
//
// After E521 `vcall18_532a17` end @ `0x532a1a`. Prep `0x532a1a..0x532a62` (72 B:
// descriptor-2 stack setup for name `0x2efd9f8`, vtable `0x15c8d98` on rbx) NOT
// modelled as pinned units in `continue_e`; this u1 runs prep + the ctor call.
// NOT modelled: 0xa54920 body; post-call vcall @ `0x532a72`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA54920_532a62_U1Rva = 0x532a62;
inline constexpr std::size_t kInit531d60PostBaseA54920_532a62_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA54920_532a62_U1EndRva = 0x532a67;
inline constexpr std::uint64_t kInit531d60PostBaseA54920_532a62_U1TargetRva = 0xa54920;
inline constexpr std::uint64_t kInit531d60PostBaseA54920_532a62_U1PrepRva = 0x532a1a;
inline constexpr std::size_t kInit531d60PostBaseA54920_532a62_U1PrepSize = 72;
inline constexpr std::uint64_t kInit531d60PostBaseA54920_532a62_U1Name2Rva = 0x2efd9f8;
inline constexpr std::uint64_t kInit531d60PostBaseA54920_532a62_U1Vtable1Rva = 0x15c8d98;

struct Init531d60PostBaseA54920_532a62_U1Hooks {
    const void* incoming_r13 = nullptr;
    void (*call_a54920)(void* parent, void* name_struct, void* descriptor) = nullptr;
};

struct Init531d60PostBaseA54920_532a62_U1Result {
    bool ok = false;
    int call_a54920_count = 0;
    int lea_rip_count = 0;
    bool a54920_hook_supplied = false;
};

[[nodiscard]] Init531d60PostBaseA54920_532a62_U1Result init_531d60_post_base_a54920_532a62_u1(
    const Init531d60PostBaseA54920_532a62_U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
