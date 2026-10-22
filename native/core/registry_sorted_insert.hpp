// Native deepen of the sorted unique-insert helpers used by registry ctor 0x52a220:
//   0x52d530 — lower_bound + unique-insert into table A (8-B entries {u32 key, u32 val})
//   0x52d660 — shift-right or grow (double) for table A; alloc via 0x17090, free via 0x170c0
//   0x52d7e0 — lower_bound + unique-insert into table B (16-B entries {u32 key, pad, ptr@+8})
//   0x52d920 — shift-right or grow (double) for table B
//
// ELF sha256 d0723afb...5861d3. Semantics OBS from offline disassembly (E389 insert path):
//   * keys sorted ascending; lower_bound finds first entry with key >= search key
//   * equal key -> return pointer to existing entry, no insert (caller may zero val)
//   * otherwise insert at that position (memmove-style shift when capacity remains, else grow)
//   * grow: empty -> 1 entry; else double byte capacity; new_array / delete_array wrappers
//
// NOT modelled: the original hint-iterator fast path (rsi); native always lower_bounds from begin
// (same unique-insert result). Stack-canary compares / __stack_chk_fail omitted (leaf policy).
// Not registered into recovered_constructors().
#pragma once
#include <cstddef>
#include <cstdint>

#include "allocator_front.hpp"

namespace sotc {

struct RegistryEntryA {
    std::uint32_t key;
    std::uint32_t val;
};
static_assert(sizeof(RegistryEntryA) == 8);

struct RegistryEntryB {
    std::uint32_t key;
    std::uint32_t pad;  // original leaves +0x4 unwritten on insert; native zeros on write
    void* ptr;          // @+0x08
};
static_assert(sizeof(RegistryEntryB) == 16);
static_assert(offsetof(RegistryEntryB, ptr) == 8);

struct RegistryVecA {
    RegistryEntryA* begin = nullptr;
    RegistryEntryA* end = nullptr;
    RegistryEntryA* cap = nullptr;
};

struct RegistryVecB {
    RegistryEntryB* begin = nullptr;
    RegistryEntryB* end = nullptr;
    RegistryEntryB* cap = nullptr;
};

// Original RVA 0x52d530 (+ grow/shift 0x52d660). Returns pointer to existing or newly inserted
// entry; nullptr if allocation fails. Does not modify an existing equal-key entry's val.
[[nodiscard]] RegistryEntryA* registry_insert_unique_a(RegistryVecA& vec, const RegistryEntryA& entry,
                                                      AllocatorFront& alloc);

// Original RVA 0x52d7e0 (+ grow/shift 0x52d920). pad is written 0; ptr copied from entry.
[[nodiscard]] RegistryEntryB* registry_insert_unique_b(RegistryVecB& vec, const RegistryEntryB& entry,
                                                      AllocatorFront& alloc);

// Release storage (native convenience; original vectors are torn down by shutdown 0x5365b0).
void registry_vec_a_clear(RegistryVecA& vec, AllocatorFront& alloc) noexcept;
void registry_vec_b_clear(RegistryVecB& vec, AllocatorFront& alloc) noexcept;

}  // namespace sotc
