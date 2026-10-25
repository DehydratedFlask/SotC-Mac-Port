// Original constructor-table index 305 / RVA 0xa77b60 (462 B), ELF d0723afb...5861d3.
// Six 16-byte destinations at 0x32fb130..0x32fb18f are copied in order: the first three
// from the guarded pair at 0x32fb190 (guard 0x32fb1a0), the last three from 0x32fb1a8
// (guard 0x32fb1b8). Both initializers call 0x51230, the already native {0,0} helper.
// Each source access performs the same ABI guard. Native C++ function-local statics supply
// that thread-safe initialization; the first pair shares static_pair_a77960 with its existing
// native consumers. No cleanup, platform service, or unresolved call is required.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "guarded_static_helpers.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot305Rva = 0xa77b60;
inline constexpr std::size_t kSlot305BodySize = 462;
inline constexpr std::size_t kSlot305PairCount = 6;
inline constexpr std::uint64_t kSlot305FirstCellRva = 0x32fb130;
inline constexpr std::uint64_t kSlot305SecondStaticRva = 0x32fb1a8;
inline constexpr std::uint64_t kSlot305SecondGuardRva = 0x32fb1b8;

using Slot305Pairs = std::array<Pair128, kSlot305PairCount>;

void init_slot305_a77b60(Slot305Pairs& pairs) noexcept;

}  // namespace sotc
