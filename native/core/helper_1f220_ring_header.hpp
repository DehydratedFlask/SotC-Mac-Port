// CPU-01-HELPER-CALLEE-MODELS-04: native model of the 27 byte leaf 0x1f220 (ELF body pinned per instruction by
// tools/lift/check_helper_1f220.py). Startup slot 473 (0x355a00) calls it at 0x3571ae with rdi = the global object 0x2ef2ba8
// and registers the destructor 0x354470 with __cxa_atexit right after.
//
//   lea rax, [rdi+0x28] ; lea rcx, [rdi+0x2628]
//   [rdi+0x20] = rax ; [rdi+0x08] = rax ; [rdi+0x00] = rax ; [rdi+0x10] = rcx ; ret
//
// Exactly four qwords are written: +0x00, +0x08 and +0x20 receive `base + 0x28` (the start of an inline 0x2600 byte
// storage area), +0x10 receives `base + 0x2628` (its end). +0x18 and everything from +0x28 on are not touched. The
// data-structure meaning (an inline-storage ring / vector header: begin / cursor / end) is INFERENCE; the stores are
// proven. Pointers are real host pointers, like the other native models. Nothing here executes an original RVA.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kRingHeader1f220Rva = 0x1f220;
inline constexpr std::size_t kRingHeader1f220Size = 27;
inline constexpr std::uint64_t kRingHeader1f220Slot473Site = 0x3571ae;
inline constexpr std::uint64_t kRingHeader1f220Slot473Object = 0x2ef2ba8;
inline constexpr std::size_t kRingHeader1f220StorageOffset = 0x28;
inline constexpr std::size_t kRingHeader1f220EndOffset = 0x2628;

// The four stores. `object` must address at least kRingHeader1f220EndOffset bytes (the storage area is only addressed).
void ring_header_init_1f220(void* object) noexcept;

}  // namespace sotc
