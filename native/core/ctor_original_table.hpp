// Original constructor table (573 entries) recovered from the private ELF.
//
// The table lives at 0x2dd70e0 and is indexed BACKWARDS: index i is the qword at
// `0x2dd70e0 - 8*i`. The ELF is pinned by sha256
// d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3; the 573-row byte string
// (index order 0..572) is pinned by sha256
// 5917cb85db190e9ef93ce5e06d5b6596d40b6c3623166e37ed7c6c3c3cd3ab28 in
// tools/lift/check_ctor_slots.py.
//
// This module is DATA ONLY. It never executes an original RVA; the RVA is the identity used to
// pair a native module with its original list position.
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace sotc {

struct OriginalCtor {
    std::uint32_t index;   // position in the original constructor list (0 -> 572)
    std::uint32_t rva;     // original ctor RVA stored at 0x2dd70e0 - 8*index (identity only)
};

// All 573 rows, in original index order. The length is kOriginalCtorCount (startup_ctors.hpp).
[[nodiscard]] std::span<const OriginalCtor> original_constructors() noexcept;

// First original index that has no row in recovered_constructors() (the production startup cliff).
// Returns -1 when every one of the 573 indices is recovered.
[[nodiscard]] int first_unrecovered_constructor_index() noexcept;

// RVA of the first unrecovered original index; 0 when every index is recovered.
[[nodiscard]] std::uint32_t first_unrecovered_constructor_rva() noexcept;

}  // namespace sotc
