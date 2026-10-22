// Native recovery of original helper RVA 0x158340 (ELF sha256 d0723afb...5861d3), task CPU-01.
// Seven-byte leaf `mov [rdi],esi; mov byte [rdi+4],0; ret` (89 37 c6 47 04 00 c3): stores the 32-bit
// argument at offset 0 and clears the byte at offset 4 of the object in rdi. Plain (non-atomic,
// unordered) stores; no return value; bytes 5..7 and everything else are untouched. Called from two
// startup ctors (0x7ec4c0, 0x10e390). Field meanings are not recovered; the names say what is written.
#pragma once
#include <cstdint>

namespace sotc {

inline constexpr unsigned kStoreValueClearFlag158340FlagOffset = 4;

// `object` points at >= 5 writable bytes (offset 0 is stored unaligned-safely via memcpy).
void store_value_clear_flag_158340(void* object, std::uint32_t value) noexcept;

}  // namespace sotc
