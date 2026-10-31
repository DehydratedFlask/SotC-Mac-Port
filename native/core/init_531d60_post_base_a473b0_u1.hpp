// Init `0x531d60` merge call-site OBS `0x5329a9..0x5329ae` (5 B):
//   call 0xa473b0   ; capture enable setter after continue C fall-through
//
// Prep `0x53299d..0x5329a9` (12 B: lea / xor esi,esi / mov rdi,[rax]) NOT modelled as
// pinned units; host stand-ins load rdi from `0x32f79a8` and esi=0 for the call.
// When `capture_services` is supplied the lifted `capture_a473b0_set_enabled(false, …)`
// runs; otherwise the raw `call_a473b0` hook. NOT modelled: cell value provenance;
// `0x5329ae` successor block. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

#include "capture_a473b0.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA473b0U1Rva = 0x5329a9;
inline constexpr std::size_t kInit531d60PostBaseA473b0U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA473b0U1EndRva = 0x5329ae;
inline constexpr std::uint64_t kInit531d60PostBaseA473b0U1TargetRva = 0xa473b0;
inline constexpr std::uint64_t kInit531d60PostBaseA473b0U1PrepRva = 0x53299d;
inline constexpr std::size_t kInit531d60PostBaseA473b0U1PrepSize = 12;
inline constexpr std::uint64_t kInit531d60PostBaseA473b0U1CellRva = 0x32f79a8;

struct Init531d60PostBaseA473b0U1Hooks {
    const void* const* cell_32f79a8 = nullptr;
    void (*call_a473b0)(void* rdi) = nullptr;
    CaptureServices* capture_services = nullptr;
};

struct Init531d60PostBaseA473b0U1Result {
    bool ok = false;
    int call_a473b0_count = 0;
    int load_count = 0;
    void* rdi_arg = nullptr;
    bool capture_native = false;
    CaptureA473b0Result capture{};
};

[[nodiscard]] Init531d60PostBaseA473b0U1Result init_531d60_post_base_a473b0_u1(
    const Init531d60PostBaseA473b0U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
