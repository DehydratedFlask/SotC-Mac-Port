// SRV-01 fixture slice for FUN_00019e00 at 0x1a123–0x1a15a.
// Raw offsets only; separate from the distinct 0x1a9bb helper.
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace sotc {

inline constexpr std::size_t kF19E00Offset80 = 0x80;
inline constexpr std::size_t kF19E00Offset88 = 0x88;
inline constexpr std::size_t kF19E00Offset90 = 0x90;
inline constexpr std::size_t kF19E00RawBytes = 0x98;

// Models only the observed qword arithmetic and branches at 0x1a123–0x1a15a.
// Returns false and leaves the span untouched unless all observed offsets fit.
bool apply_f19e00_accounting(std::span<std::byte> raw,
                             std::uint64_t requested_bytes,
                             std::uint32_t mode) noexcept;

}  // namespace sotc
