// Init `0x531d60` merge call-site OBS `0x532946..0x53294b` (5 B):
//   call 0xa599e0   ; first obj-A setter after shrunk continue_c entry
//
// Prep `0x532910..0x532946` (54 B: lea/xor/vcall/flag/sete chain) NOT modelled as
// pinned units; host stand-ins reproduce rdi=obj A and esi=sil for the call hook.
// NOT modelled: 0xa599e0 body beyond hook. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lazy_bind_91210.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA599e0U1Rva = 0x532946;
inline constexpr std::size_t kInit531d60PostBaseA599e0U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA599e0U1EndRva = 0x53294b;
inline constexpr std::uint64_t kInit531d60PostBaseA599e0U1TargetRva = 0xa599e0;
inline constexpr std::uint64_t kInit531d60PostBaseA599e0U1PrepRva = 0x532910;
inline constexpr std::size_t kInit531d60PostBaseA599e0U1PrepSize = 54;

struct Init531d60PostBaseA599e0U1Hooks {
    const void* const* cell_32f8b80 = nullptr;
    const void* const* cell_2efe318 = nullptr;
    const std::uint8_t* flag_byte_iter1 = nullptr;
    void (*vcall_slot0)(void* rdi, std::uint8_t esi) = nullptr;
    void (*call_a599e0)(void* rdi, std::uint8_t esi) = nullptr;
    bool use_native_lazy_bind_91210 = false;
    void* demo_object = nullptr;
    LazyBind91210Hooks lazy_bind{};
};

struct Init531d60PostBaseA599e0U1Result {
    bool ok = false;
    int call_a599e0_count = 0;
    void* rdi_arg = nullptr;
    std::uint8_t esi_arg = 0;
    bool flag1_known = false;
    std::uint8_t sil_iter1 = 0;
    int vcall_count = 0;
    int lazy_bind_calls = 0;
    bool lazy_bind_ok = false;
};

[[nodiscard]] Init531d60PostBaseA599e0U1Result init_531d60_post_base_a599e0_u1(
    const Init531d60PostBaseA599e0U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
