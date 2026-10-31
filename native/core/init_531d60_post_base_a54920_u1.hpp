// Init `0x531d60` continue E first call-site OBS `0x532a07..0x532a0c` (5 B):
//   call 0xa54920   ; save-data ctor 1 ("save_%s_max_count")
//
// Branch-target entry @ `0x5329ae` (batch C merge je taken). Prep
// `0x5329ae..0x532a07` (89 B: boss-flag lea, descriptor setup, r12b stamp) NOT
// modelled as pinned units in `continue_e`; this u1 runs the prep semantics and
// the first ctor call. NOT modelled: 0xa54920 body beyond hook; vcall after call.
// G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA54920U1Rva = 0x532a07;
inline constexpr std::size_t kInit531d60PostBaseA54920U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA54920U1EndRva = 0x532a0c;
inline constexpr std::uint64_t kInit531d60PostBaseA54920U1TargetRva = 0xa54920;
inline constexpr std::uint64_t kInit531d60PostBaseA54920U1PrepRva = 0x5329ae;
inline constexpr std::size_t kInit531d60PostBaseA54920U1PrepSize = 89;
inline constexpr std::uint64_t kInit531d60PostBaseA54920U1BossFlagRva = 0x2dcdf14;
inline constexpr std::uint64_t kInit531d60PostBaseA54920U1Name1Rva = 0x2efd9a0;
inline constexpr std::uint64_t kInit531d60PostBaseA54920U1Vtable1Rva = 0x15c8d98;
inline constexpr std::uint64_t kInit531d60PostBaseContinueEEntryRva = 0x5329ae;

struct Init531d60PostBaseA54920U1Hooks {
    const void* incoming_r13 = nullptr;
    std::uint8_t incoming_r12b = 0;
    std::uint8_t* boss_flag_2dcdf14 = nullptr;
    void (*call_a54920)(void* parent, void* name_struct, void* descriptor) = nullptr;
};

struct Init531d60PostBaseA54920U1Result {
    bool ok = false;
    int call_a54920_count = 0;
    int stamp_count = 0;
    int lea_rip_count = 0;
    std::uint8_t boss_flag_before = 0;
    std::uint8_t boss_flag_after = 0;
    bool a54920_hook_supplied = false;
};

[[nodiscard]] Init531d60PostBaseA54920U1Result init_531d60_post_base_a54920_u1(
    const Init531d60PostBaseA54920U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
