// Init `0x531d60` continue D — OBS `0x53299d..0x5329ae` (17 B, 4 units).
//
// Fall-through of batch C's `je 0x5329ae` (je NOT taken):
//   0x53299d  lea rax,[0x32f79a8]   (7 B)
//   0x5329a4  xor esi,esi           (2 B)
//   0x5329a6  mov rdi,[rax]         (3 B)
//   0x5329a9  call 0xa473b0         (5 B)
// and then control reaches `0x5329ae`, the je-taken block, which is NOT modelled here.
//
// Claimed: the 17 B span, the RIP-relative lea target, the call rel32 target, and the
// ordering (cell load before the call).
// NOT modelled: the body of 0xa473b0; the value held by the 0x32f79a8 cell (host stand-in);
// the `0x5329ae` successor block; the merge `je` outcome; r15/other incoming registers.
// G1 PARTIAL — control-flow pin only.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueDRva = 0x53299d;
inline constexpr std::size_t kInit531d60PostBaseContinueDSize = 17;
inline constexpr std::uint64_t kInit531d60PostBaseContinueDEndRva = 0x5329ae;
inline constexpr std::uint64_t kInit531d60PostBaseContinueDCellRva = 0x32f79a8;
inline constexpr std::uint64_t kInit531d60PostBaseContinueDCallRva = 0xa473b0;

enum class ContinueDUnit : int {
    lea_rax_32f79a8 = 0,  // 0x53299d
    xor_esi_esi,          // 0x5329a4
    mov_rdi_cell_load,    // 0x5329a6
    call_a473b0,          // 0x5329a9
    count
};
inline constexpr int kInit531d60PostBaseContinueDUnitCount =
    static_cast<int>(ContinueDUnit::count);

struct ContinueDUnitSpan {
    std::uint64_t rva = 0;
    std::uint8_t size = 0;
    const char* mnemonic = "";
};

inline constexpr std::array<ContinueDUnitSpan, kInit531d60PostBaseContinueDUnitCount>
    kInit531d60PostBaseContinueDUnits{{
        {0x53299d, 7, "lea rax,[0x32f79a8]"},
        {0x5329a4, 2, "xor esi,esi"},
        {0x5329a6, 3, "mov rdi,[rax]"},
        {0x5329a9, 5, "call 0xa473b0"},
    }};

struct Init531d60PostBaseContinueDHooks {
    // [0x32f79a8] load stand-in; null => rdi is null (load still counted).
    const void* const* cell_32f79a8 = nullptr;
    // 0xa473b0(rdi, esi=0); null => no-op, call still counted.
    void (*call_a473b0)(void* rdi) = nullptr;
};

struct Init531d60PostBaseContinueDResult {
    bool ok = false;
    std::array<int, kInit531d60PostBaseContinueDUnitCount> unit_counts{};
    int unit_executed = 0;   // expect 4
    int call_count = 0;      // expect 1
    int load_count = 0;      // expect 1
    std::uint64_t end_rva = kInit531d60PostBaseContinueDEndRva;
};

[[nodiscard]] Init531d60PostBaseContinueDResult init_531d60_post_base_continue_d(
    const Init531d60PostBaseContinueDHooks& hooks = {}) noexcept;

}  // namespace sotc
