// CPU-01-SLOT408-PREREQS: native models of two single-pointer "cell" destructors (ELF bodies pinned per instruction by
// tools/lift/check_helper_534580.py). Both own exactly one pointer at [object]. Free / virtual-delete are HOOKS: 0x170c0 is
// `mov esi,2 ; jmp 0x12af0` (the TLS allocator dispatch, an indirect call) and the 0x13a0d0 delete is `call [[p]+8]`.
//
//   0x534580 (14 B, startup slot 427's zero-container dtor): p = [rdi] ; if (p) jmp 0x170c0 ; ret      -- cell NOT cleared
//   0x13a0d0 (37 B, startup slot 534): p = [rbx] ; if (p) { call [[p]+8] ; [rbx] = 0 }               -- cell cleared
//
// Neither is run by a startup slot (dtors are atexit identity tags); these models make the cell semantics testable.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kCellDtor534580Rva = 0x534580;
inline constexpr std::size_t kCellDtor534580Size = 14;
inline constexpr std::uint64_t kCellDtor13a0d0Rva = 0x13a0d0;
inline constexpr std::size_t kCellDtor13a0d0Size = 37;
inline constexpr std::uint64_t kCellDtorFreeType2Rva = 0x170c0;
inline constexpr std::size_t kCellDtor13a0d0VtableSlotOffset = 0x08;

struct CellDtorHooks {
    void (*free_type2)(void* pointer) = nullptr;       // 0x170c0
    void (*virtual_delete)(void* pointer) = nullptr;   // [[pointer]+8]
};

// Returns true when the body ran to completion (a non-null pointer with its hook missing reports false and does nothing).
[[nodiscard]] bool cell_dtor_534580(void* const* cell, const CellDtorHooks& hooks) noexcept;
[[nodiscard]] bool cell_dtor_13a0d0(void** cell, const CellDtorHooks& hooks) noexcept;

}  // namespace sotc
