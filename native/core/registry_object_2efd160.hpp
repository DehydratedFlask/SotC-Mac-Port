// Native deepen of the 0x98-byte hash-keyed registry object stored at global 0x2efd160.
// Built by Init 0x531d60 via alloc 0x17070 + ctor 0x52a220(obj, input_table_base, count=275).
// ELF sha256 d0723afb...5861d3. Layout from offline analysis (E389); not a full body lift of 0x52a220.
//
// This module recovers ONLY the prologue stores of 0x52a220 (zero the three vector heads, stash the
// input-table pointer, write float immediates 5.0f @ +0x6c and 60.0f @ +0x80). Sorted-table insert
// loops, listener vectors, pool globals 0x2efd170/190/1b0, and name hashing into +0x58/+0x5c are
// NOT modelled here — they need the path-FNV helper and further insert lifts.
//
// Allocator 0x17070 does not zero-fill; bytes not written by the prologue remain as left by the
// caller (tests start from a poison pattern). Not registered into recovered_constructors().
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kRegistryObject2efd160Bytes = 0x98;
inline constexpr std::uint32_t kRegistryObjectFloat5 = 0x40a00000u;   // 5.0f @ +0x6c
inline constexpr std::uint32_t kRegistryObjectFloat60 = 0x42700000u;  // 60.0f @ +0x80

struct RegistryObject2efd160 {
    // +0x00 / +0x08 / +0x10 — sorted vector<{u32 key, u32 val}> begin/end/cap (8-B entries)
    void* table_a_begin;
    void* table_a_end;
    void* table_a_cap;
    std::uint64_t pad_18;  // not written by the prologue
    // +0x20 / +0x28 / +0x30 — sorted vector<{u32 key, pad, ptr@+8}> begin/end/cap (16-B entries)
    void* table_b_begin;
    void* table_b_end;
    void* table_b_cap;
    std::uint64_t pad_38;
    // +0x40 / +0x48 / +0x50 — listener-group vector head (populated at runtime)
    void* listeners_begin;
    void* listeners_end;
    void* listeners_cap;
    std::uint64_t pad_58;
    // +0x60 — input table base (original: 0x2eff8e0)
    void* input_table;
    std::uint32_t pad_68;   // not written by the prologue
    std::uint32_t float_5;  // +0x6c
    std::uint64_t pad_70;   // +0x70..+0x77 not written by the prologue
    std::uint64_t pad_78;   // +0x78..+0x7f
    std::uint32_t float_60; // +0x80
    std::uint32_t pad_84;
    std::uint64_t pad_88;
    std::uint64_t pad_90;
};
static_assert(sizeof(RegistryObject2efd160) == kRegistryObject2efd160Bytes);
static_assert(offsetof(RegistryObject2efd160, input_table) == 0x60);
static_assert(offsetof(RegistryObject2efd160, float_5) == 0x6c);
static_assert(offsetof(RegistryObject2efd160, float_60) == 0x80);

// Prologue of 0x52a220: zero the three vector heads, store input_table, write the two float immediates.
// Does not hash names or insert into the sorted tables.
void init_registry_object_header_52a220(RegistryObject2efd160& obj, void* input_table) noexcept;

}  // namespace sotc
