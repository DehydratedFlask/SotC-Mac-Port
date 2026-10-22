// Native deepen of the numeric half of original ctor RVA 0x551140 (exec index 424;
// ELF sha256 d0723afb...5861d3). The original fills a 275-entry BSS table at 0x2eff8e0
// (stride 0x60 = 96 B). After three pre-objects with calls, a pure store region
// 0x55141d..0x557fe4 (27591 B, 2088 stores, 0 calls/branches) writes every entry.
//
// This module emits ONLY the immediate numeric fields recovered from that region:
//   +0x10 u32 a (0..5), +0x14 u32 b (0..3), +0x18 u32 c (0..3), +0x50 u64 f50 (0..15).
// Name string pointers (+0x00/+0x08) and callback/vtable state (+0x20/+0x28/+0x40) are
// left null — original name bytes and callable bodies stay private / out of scope.
// Accessors 0x54dc10 (returns base) and 0x54dc20 (returns 0x113 = 275) are modelled.
//
// Not registered into recovered_constructors() (index 424 is past the production cliff).
// Registry writer Init 0x531d60 / global 0x2efd160 still consume name keys — those remain
// blocked on a private name fixture, not this numeric deepen. Goal incomplete; G1 NOT RUN.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

#include "input_table_551140_numeric.inc.hpp"

namespace sotc {

inline constexpr std::size_t kInputTable551140Count = input_table_551140_detail::kCount;
inline constexpr std::size_t kInputTable551140Stride = 0x60;
inline constexpr std::uint32_t kInputTable551140CountU32 = 0x113u;  // accessor 0x54dc20

struct InputTableEntry551140 {
    const char* name_a;       // +0x00  original: rodata key; null here
    const char* name_b;       // +0x08  original: rodata name; null here
    std::uint32_t a;          // +0x10
    std::uint32_t b;          // +0x14
    std::uint32_t c;          // +0x18
    std::uint32_t pad_1c;     // +0x1c  never stored by the region
    void* callback;           // +0x20  original: vptr / 0; null here
    void* fnptr_state;        // +0x28  original: captured fn / 0; null here
    std::uint64_t zero_30;    // +0x30  never stored
    std::uint64_t zero_38;    // +0x38  never stored
    void* self_or_null;       // +0x40  original: entry+0x20 when callable else 0; null here
    std::uint64_t zero_48;    // +0x48  never stored
    std::uint64_t f50;        // +0x50
    std::uint32_t zero_58;    // +0x58  stored as 0
    std::uint32_t zero_5c;    // +0x5c  stored as 0
};
static_assert(sizeof(InputTableEntry551140) == kInputTable551140Stride);
static_assert(offsetof(InputTableEntry551140, a) == 0x10);
static_assert(offsetof(InputTableEntry551140, b) == 0x14);
static_assert(offsetof(InputTableEntry551140, c) == 0x18);
static_assert(offsetof(InputTableEntry551140, callback) == 0x20);
static_assert(offsetof(InputTableEntry551140, f50) == 0x50);

using InputTable551140 = std::array<InputTableEntry551140, kInputTable551140Count>;

// Fill every entry from the generated numeric rows; names and callbacks stay null.
// Idempotent: every modelled field is overwritten unconditionally.
void init_input_table_551140(InputTable551140& table) noexcept;

// Models accessor 0x54dc10 — returns &table[0] (original: image address 0x2eff8e0).
[[nodiscard]] InputTableEntry551140* input_table_551140_base(InputTable551140& table) noexcept;
[[nodiscard]] const InputTableEntry551140* input_table_551140_base(
    const InputTable551140& table) noexcept;

// Models accessor 0x54dc20 — returns 0x113 (=275).
[[nodiscard]] std::uint32_t input_table_551140_count() noexcept;

}  // namespace sotc
