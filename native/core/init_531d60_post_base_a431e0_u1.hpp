// Init `0x531d60` merge call-site OBS `0x53297a..0x53297f` (5 B):
//   call 0xa431e0   ; obj-B setter after shrunk continue_c entry
//
// Prep `0x53294b..0x53297a` (47 B: lea/xor/vcall/flag/sete chain) NOT modelled as
// pinned units; host stand-ins reproduce rdi=obj B and esi=sil for the call hook.
// NOT modelled: 0xa431e0 body beyond hook. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lazy_bind_91210.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA431e0U1Rva = 0x53297a;
inline constexpr std::size_t kInit531d60PostBaseA431e0U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA431e0U1EndRva = 0x53297f;
inline constexpr std::uint64_t kInit531d60PostBaseA431e0U1TargetRva = 0xa431e0;
inline constexpr std::uint64_t kInit531d60PostBaseA431e0U1PrepRva = 0x53294b;
inline constexpr std::size_t kInit531d60PostBaseA431e0U1PrepSize = 47;

struct Init531d60PostBaseA431e0U1Hooks {
    const void* const* cell_32f6d18 = nullptr;
    const void* const* cell_2efe318 = nullptr;
    const std::uint8_t* flag_byte_iter2 = nullptr;
    void (*vcall_slot0)(void* rdi, std::uint8_t esi) = nullptr;
    void (*call_a431e0)(void* rdi, std::uint8_t esi) = nullptr;
    bool use_native_lazy_bind_91210 = false;
    void* demo_object = nullptr;
    LazyBind91210Hooks lazy_bind{};
};

struct Init531d60PostBaseA431e0U1Result {
    bool ok = false;
    int call_a431e0_count = 0;
    void* rdi_arg = nullptr;
    std::uint8_t esi_arg = 0;
    bool flag2_known = false;
    std::uint8_t sil_iter2 = 0;
    int vcall_count = 0;
    int lazy_bind_calls = 0;
    bool lazy_bind_ok = false;
};

[[nodiscard]] Init531d60PostBaseA431e0U1Result init_531d60_post_base_a431e0_u1(
    const Init531d60PostBaseA431e0U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
