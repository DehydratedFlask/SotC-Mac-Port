// Init `0x531d60` continue D — OBS boundary @ `0x5329ae` (0 B pinned units after E519).
//
// Fall-through of batch C's `je 0x5329ae` (je NOT taken). The 17 B ELF span
// `0x53299d..0x5329ae` is split: prep + `call 0xa473b0` run via `a473b0_u1`; this
// module pins only the post-call boundary (batch E entry @ `0x5329ae`).
//
// Claimed: boundary RVA, chaining after u1, and u1 merge counters on the D result.
// NOT modelled: `0x5329ae` successor block, merge `je` outcome, incoming registers.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "capture_a473b0.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueDRva = 0x5329ae;
inline constexpr std::size_t kInit531d60PostBaseContinueDSize = 0;
inline constexpr std::uint64_t kInit531d60PostBaseContinueDEndRva = 0x5329ae;
inline constexpr std::uint64_t kInit531d60PostBaseContinueDCellRva = 0x32f79a8;
inline constexpr std::uint64_t kInit531d60PostBaseContinueDCallRva = 0xa473b0;
inline constexpr std::uint64_t kInit531d60PostBaseContinueDFullSpanRva = 0x53299d;
inline constexpr std::size_t kInit531d60PostBaseContinueDFullSpanSize = 17;

enum class ContinueDUnit : int { count = 0 };
inline constexpr int kInit531d60PostBaseContinueDUnitCount =
    static_cast<int>(ContinueDUnit::count);

struct ContinueDUnitSpan {
    std::uint64_t rva = 0;
    std::uint8_t size = 0;
    const char* mnemonic = "";
};

struct Init531d60PostBaseContinueDHooks {
    const void* const* cell_32f79a8 = nullptr;
    void (*call_a473b0)(void* rdi) = nullptr;
    CaptureServices* capture_services = nullptr;
    bool merge_a473b0_u1_ran = false;
};

struct Init531d60PostBaseContinueDResult {
    bool ok = false;
    std::array<int, 1> unit_counts{};  // unused; kept for stable result shape
    int unit_executed = 0;   // expect 0
    int call_count = 0;      // expect 1 (from u1)
    int load_count = 0;      // expect 0..1 (from u1)
    std::uint64_t end_rva = kInit531d60PostBaseContinueDEndRva;
    bool capture_native = false;
    CaptureA473b0Result capture{};
};

[[nodiscard]] Init531d60PostBaseContinueDResult init_531d60_post_base_continue_d(
    const Init531d60PostBaseContinueDHooks& hooks = {}) noexcept;

}  // namespace sotc
