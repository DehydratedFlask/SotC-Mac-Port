// Native leaf for OBS helper RVA 0x323d0 (67 B; sha 32da90bc…).
// Called from heavy ctor 0x3be050 with rdi = obj+0x98a88.
//
// OBS control flow:
//   if (*0x2ea4700 == 0) {
//     rax = call 0xf8f860;   // PLT — NOT modelled (hook / default sentinel)
//     *0x2ea4700 = rax;
//     *0x2ea4708 = rdtsc64;
//   }
//   *rdi = rdtsc64;
//
// Host: rdtsc modelled with mach_absolute_time() (Apple Silicon; not x86 TSC).
// No leaf-vec; not idx 95; PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kStoreTsc323d0Rva = 0x323d0;
inline constexpr std::size_t kStoreTsc323d0Size = 67;
inline constexpr std::uint64_t kStoreTscGuardCellRva = 0x2ea4700;
inline constexpr std::uint64_t kStoreTscGlobalCellRva = 0x2ea4708;

struct StoreTsc323d0Hooks {
    // Models call 0xf8f860 (PLT). Null → write non-zero sentinel 1 (body NOT claimed).
    std::uint64_t (*init_timebase_f8f860)() = nullptr;
};

// Process-wide cells modelling OBS qwords @ 0x2ea4700 / 0x2ea4708 (BSS).
[[nodiscard]] std::uint64_t& store_tsc_guard_cell_2ea4700() noexcept;
[[nodiscard]] std::uint64_t& store_tsc_global_cell_2ea4708() noexcept;

// Host stand-in for OBS rdtsc (edx:eax → u64).
[[nodiscard]] std::uint64_t host_rdtsc64() noexcept;

// Models 0x323d0. Writes 8 bytes at `dest`.
void store_tsc_323d0(void* dest, const StoreTsc323d0Hooks& hooks = {}) noexcept;

}  // namespace sotc
