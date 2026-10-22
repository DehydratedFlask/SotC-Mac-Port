// Heavy ctor `0x3be050` for the 0x98a90 object installed at app+0x3b58 (prologue tail).
//
// Full OBS body: 0x3be050..0x3be2c2 (627 B; sha f186db1c…). Next symbol at 0x3be2d0.
//
// Lifted (tip AllocatorFront):
//   Header OBS 0x3be050..0x3be0b6 (102 B): nested 0x48 sentinel + field zeros.
//   Ensure 0x3c2f70(obj+0x20, 0x100) when capacity elems <=0xff (lifted empty path).
//   Buffer grow OBS 0x3be0f8..0x3be1cb: 0x80 vec at +0x48/+0x50/+0x58.
//   List-walk head: cells 0x2ef63f8 / 0x2ef6400; null → skip body.
//   TSC leaf 0x323d0 at obj+0x98a88 (host mach_absolute_time).
//
// Hooks (bodies NOT modelled):
//   0x3be930(obj+0x68); ensure migrate (0x1da30/0x170c0); 0xf590; 0xf8f860 PLT;
//   non-null list body.
//
// Prefer ensure 0x3c2f70 over 0x3be930 (826 B). No leaf-vec; not idx 95; PR #5 cite-only.
#pragma once

#include "allocator_front.hpp"
#include "ensure_capacity_3c2f70.hpp"
#include "store_tsc_323d0.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kCtor98a90_3be050Rva = 0x3be050;
inline constexpr std::size_t kCtor98a90_3be050Size = 627;
inline constexpr std::size_t kCtor98a90_3be050HeaderSize = 102;
inline constexpr std::size_t kCtor98a90_3be050BufGrowSize = 212;
inline constexpr std::size_t kCtor98a90_3be050ListHeadSize = 24;
inline constexpr std::size_t kCtor98a90NestedBytes = 0x48;
inline constexpr std::size_t kCtor98a90BufBytes = 0x80;
inline constexpr std::size_t kCtor98a90ElemBytes = 0x20;
inline constexpr std::size_t kCtor98a90TscOffset = 0x98a88;
inline constexpr std::size_t kCtor98a90VecBegin = 0x48;
inline constexpr std::size_t kCtor98a90VecEnd = 0x50;
inline constexpr std::size_t kCtor98a90VecCap = 0x58;
inline constexpr std::size_t kCtor98a90EnsureVec = 0x20;
inline constexpr std::uint64_t kCtor98a90SelfCellRva = 0x2ef63f8;
inline constexpr std::uint64_t kCtor98a90ListHeadCellRva = 0x2ef6400;

struct Ctor98a90_3be050Hooks {
    void (*sub_3be930)(void* at_plus_68) = nullptr;
    void (*store_tsc_323d0)(void* at_plus_98a88) = nullptr;
    StoreTsc323d0Hooks tsc_detail{};
    // If non-null, replaces lifted ensure_capacity_3c2f70.
    void (*ensure_3c2f70)(void* at_plus_20, std::uint32_t n) = nullptr;
    EnsureCapacity3c2f70Hooks ensure_detail{};
    void (*elem_dtor_f590)(void* elem) = nullptr;
    void (*list_walk_nonnull_body)(void* obj, void* list_head) = nullptr;
};

struct Ctor98a90_3be050Result {
    bool ok = false;
    void* nested_48 = nullptr;
    bool ensure_called = false;
    bool ensure_allocated = false;
    void* ensure_buf = nullptr;
    bool buf_grown = false;
    void* buf_80 = nullptr;
    bool list_self_stored = false;
    bool list_walk_skipped_null = false;
    void* list_head_seen = nullptr;
    bool tsc_stored = false;
    std::uint64_t tsc_value = 0;
};

[[nodiscard]] void*& ctor_98a90_self_cell_2ef63f8() noexcept;
[[nodiscard]] void*& ctor_98a90_list_head_cell_2ef6400() noexcept;

[[nodiscard]] Ctor98a90_3be050Result init_98a90_3be050(AllocatorFront& alloc, void* obj,
                                                       const Ctor98a90_3be050Hooks& hooks);

}  // namespace sotc
