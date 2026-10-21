#pragma once

#include <cstdint>

namespace sotc {

// Name hash used by the original engine's string-keyed lookup (djb2 with XOR, offset 5381).
// Observed in the dedicated routine at RVA 0xe70aa0 and recomputed against 13 constructor
// constants at RVA 0xe71990 (see docs/checkpoints/2026-10-21-agentB-namehash-e71990.md).
//
//   h = 5381; for each byte c: h = (h * 33) ^ sign_extend32(c)
//
// Quirks of the dedicated routine: a null pointer yields 0, bytes are sign-extended (matching
// `movsx`), and a final hash of 0 is remapped to 1 so that 0 can mean "no name".
[[nodiscard]] std::uint32_t name_hash(const char* name) noexcept;

// The same loop as it appears inlined at hash-table call sites: bytes are zero-extended and the
// result is neither null-checked nor remapped. Callers mask it with (bucket_count - 1).
[[nodiscard]] std::uint32_t name_hash_inline(const char* name) noexcept;

}  // namespace sotc
