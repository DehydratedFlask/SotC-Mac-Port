// Init `0x531d60` singleton install OBS `0x531d84..0x531dcf` (75 B):
//   edi = 0xf08; call 0x17070          // alloc
//   call 0x523bd0(obj)                 // ctor (huge — NOT modelled)
//   lea cell → 0x2efc9e0; cmp/swap:
//     if prev != neu && prev: call 0x525b40; call 0x170b0
//     store neu → cell
//   then Init calls base Initialize 0x255130 @ 0x531dda
//
// Always-on on measured Init path before base Initialize. Prefer over ensure
// migrate (0x1da30/0x170c0/0xf590 — empty-path skipped) and bounded 0x533100
// header (later vslot; not this edge).
//
// Claimed: install control flow + alloc size + cell pin + callee sites.
// NOT modelled: bodies of 0x523bd0 / 0x525b40 (hooks only); 0x533100; heap/leaf-vec;
// idx 95. PR #5 cite-only. No leaf-vec; not idx 95.
#pragma once

#include "allocator_front.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kSingletonF08InstallRva = 0x531d84;
inline constexpr std::size_t kSingletonF08InstallSize = 75;  // through store @ 0x531dcc
inline constexpr std::size_t kSingletonF08ObjectBytes = 0xf08;
inline constexpr std::uint64_t kSingletonF08CellRva = 0x2efc9e0;
inline constexpr std::uint64_t kSingletonF08CtorRva = 0x523bd0;
inline constexpr std::uint64_t kSingletonF08DtorRva = 0x525b40;

struct SingletonF08Hooks {
    // Models 0x523bd0(rdi=obj). Null → zero-fill object (safe delete_ on replace).
    void (*ctor_523bd0)(void* obj) = nullptr;
    // Models 0x525b40(rdi=prev) before delete_. Null → skip (still delete_).
    void (*dtor_525b40)(void* obj) = nullptr;
};

struct SingletonF08Result {
    bool ok = false;
    bool alloc_failed = false;
    bool reused_same = false;
    bool destroyed_prev = false;
    bool ctor_ran = false;
    void* obj = nullptr;
};

// Process-wide cell modelling original qword @ 0x2efc9e0.
[[nodiscard]] void*& singleton_f08_cell_2efc9e0() noexcept;

// Destroy *cell (dtor hook + delete_) and store nullptr. Safe if already null.
void destroy_singleton_f08_at_2efc9e0(AllocatorFront& alloc,
                                      const SingletonF08Hooks& hooks = {}) noexcept;

// OBS install fragment. On alloc failure cell is unchanged. Same-pointer fast path
// skips destroy+store (matches je @ 0x531db5).
[[nodiscard]] SingletonF08Result install_singleton_f08_531d84(
    AllocatorFront& alloc, const SingletonF08Hooks& hooks = {}) noexcept;

}  // namespace sotc
