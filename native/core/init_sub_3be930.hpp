// Native lift for OBS sub-init `0x3be930` (826 B; sha e3cfc0d4…).
// Called from heavy ctor 0x3be050 with rdi = obj+0x68.
//
// OBS: zero [base]/[base+4]; then init_block_1d9c0 over a packed 0x98-stride
// layout (first at +0x10, loop +0xa8..+0x98a8, then 15×(single+256-bank));
// finally zero dword at +0x98a10 and qword at +0x98a18.
//
// Leaves:
//   0x1d9c0 (73 B): slot ctor — calls 0xf180(obj+8), installs internal ptrs, zeros.
//   0xf180 (28 B): store host stand-in for lea 0x2dd9f78; zero +8 xmm and +0x18.
//
// Prefer over 0x1ab550 / debug gates / vslot / 0xf08. No leaf-vec; not idx 95;
// PR #5 cite-only. Tip AllocatorFront not required (no heap in this body).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInitSub3be930Rva = 0x3be930;
inline constexpr std::size_t kInitSub3be930Size = 826;
inline constexpr std::uint64_t kInitBlock1d9c0Rva = 0x1d9c0;
inline constexpr std::size_t kInitBlock1d9c0Size = 73;
inline constexpr std::uint64_t kInitSlotF180Rva = 0xf180;
inline constexpr std::size_t kInitSlotF180Size = 28;
inline constexpr std::uint64_t kSlotVtableRva = 0x2dd9f78;
inline constexpr std::size_t kInitBlock1d9c0Bytes = 0x98;
inline constexpr std::size_t kInitSub3be930OffInObj = 0x68;

// Host stand-in for OBS lea target 0x2dd9f78 (BSS vtable/type blob address).
[[nodiscard]] void* slot_vtable_2dd9f78() noexcept;

// Models 0xf180(rdi=at).
void init_slot_f180(void* at) noexcept;

// Models 0x1d9c0(rdi=obj) — obj sized >= 0x98.
void init_block_1d9c0(void* obj) noexcept;

// Models 0x3be930(rdi=at_plus_68). Region through +0x98a20 must be writable.
void init_sub_3be930(void* at_plus_68) noexcept;

}  // namespace sotc
