// Native recovery of original function RVA 0x125990 (ELF sha256 d0723afb...5861d3), task CPU-01.
// Second recovered function and the first with a real call (libc memset). Names are ours.
#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kScratchBytes = 0x2000;   // the length operand of the original memset

// Original: memset(buffer, 0, 0x2000); then `xchg [slot], rbx` with rbx == 0 (old value discarded),
// i.e. a sequentially consistent store of zero to a 64-bit slot.
// The original's two globals are 0x2008 bytes apart (buffer start -> slot); that layout fact is
// recorded in local/re/functions.jsonl, not baked into this interface.
void reset_scratch(std::array<std::byte, kScratchBytes>& buffer, std::atomic<std::uint64_t>& slot) noexcept;

}  // namespace sotc
