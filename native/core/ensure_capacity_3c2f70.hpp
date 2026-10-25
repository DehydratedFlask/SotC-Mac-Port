// Native lift for OBS ensure `0x3c2f70` (313 B; sha 7f9ed966…).
// Called from heavy ctor 0x3be050 with rdi=obj+0x20, esi=0x100 when vec
// capacity ( ([+0x38]-[+0x28]) / 0xa8 ) <= 0xff.
//
// Vec at `vec` (= obj+0x20):
//   +0x08 begin, +0x10 end, +0x18 capacity_end; element stride 0xa8.
//
// Measured empty path (begin==end==null): alloc n*0xa8, install
//   begin=new, end=new, cap=new+n*0xa8. Migrate/destroy NOT taken.
//
// Hooks (NOT modelled bodies): 0x1da30 elem move; 0x170c0 inner free; 0xf590 dtor.
// Overflow (n > 0x186186186186186) → fail (OBS abort PLT 0xf8f5f0 + ud2 NOT run).
//
// Prefer over 0x3be930 (826 B). No leaf-vec; not idx 95; PR #5 cite-only.
#pragma once

#include "allocator_front.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kEnsureCapacity3c2f70Rva = 0x3c2f70;
inline constexpr std::size_t kEnsureCapacity3c2f70Size = 313;
inline constexpr std::size_t kEnsure3c2f70ElemBytes = 0xa8;
inline constexpr std::uint64_t kEnsure3c2f70MaxElems = 0x186186186186186ULL;
inline constexpr std::uint64_t kEnsure3c2f70MagicDiv = 0xcf3cf3cf3cf3cf3dULL;  // /0xa8
inline constexpr std::size_t kEnsure3c2f70OffBegin = 0x08;
inline constexpr std::size_t kEnsure3c2f70OffEnd = 0x10;
inline constexpr std::size_t kEnsure3c2f70OffCap = 0x18;

struct EnsureCapacity3c2f70Hooks {
    // OBS call 0x1da30(dst+8, src+8) during migrate. Null → memcpy 0xa0-8? skip body fields.
    void (*elem_move_1da30)(void* dst_plus_8, void* src_plus_8) = nullptr;
    void (*elem_inner_free_170c0)(void* p) = nullptr;  // from [elem+0x38] path
    void (*elem_dtor_f590)(void* elem) = nullptr;
};

struct EnsureCapacity3c2f70Result {
    bool ok = false;
    bool allocated = false;
    void* buf = nullptr;
};

// OBS capacity count: ((end-begin)>>3) * magic  (== (end-begin)/0xa8 for aligned diffs).
[[nodiscard]] inline std::uint64_t ensure_3c2f70_elem_count(std::uint64_t begin,
                                                            std::uint64_t end) noexcept {
    const std::uint64_t diff = end - begin;
    return ((diff >> 3) * kEnsure3c2f70MagicDiv);
}

[[nodiscard]] EnsureCapacity3c2f70Result ensure_capacity_3c2f70(
    AllocatorFront& alloc, void* vec, std::uint64_t n,
    const EnsureCapacity3c2f70Hooks& hooks = {}) noexcept;

}  // namespace sotc
