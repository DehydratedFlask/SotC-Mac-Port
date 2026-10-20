// SRV-01: intrusive list surgery after region split (OBS 0x1a902..0x1a961,
// unlink+relink 0x1a974..0x1a9b7). Links live at RegionNode+0x20.
#pragma once

#include <cstdint>

#include "region_split.hpp"

namespace sotc {

// Embedded list node at RegionNode +0x20 (OBS):
//   +0x00 identity (node base)
//   +0x08 prev  (IntrusiveLinks*)
//   +0x10 next  (IntrusiveLinks*)
// RegionNode +0x38 stores list owner/sentinel pointer.
struct IntrusiveLinks {
    void* identity = nullptr;
    IntrusiveLinks* prev = nullptr;
    IntrusiveLinks* next = nullptr;
};

static_assert(sizeof(IntrusiveLinks) == 24);

inline IntrusiveLinks* region_links(RegionNode& n) noexcept {
    return reinterpret_cast<IntrusiveLinks*>(n.raw + 0x20);
}
inline const IntrusiveLinks* region_links(const RegionNode& n) noexcept {
    return reinterpret_cast<const IntrusiveLinks*>(n.raw + 0x20);
}
inline void*& region_owner(RegionNode& n) noexcept {
    return *reinterpret_cast<void**>(n.raw + 0x38);
}

// 0x1a974..0x1a98f: splice out; leave next self-looped to own links.
void list_unlink(RegionNode& node) noexcept;

// 0x1a913..0x1a92f + 0x1a961: insert `neu` before `successor`; set owner.
void list_insert_before(RegionNode& neu, RegionNode& successor, void* owner) noexcept;

// 0x1a997..0x1a9b7: insert at allocated-list head (arena +0x10 holds next Links*).
// OBS also stores arena pointer into neu.prev (+0x28) and owner (+0x38).
void list_insert_at_head(RegionNode& neu, IntrusiveLinks** head_next_slot, void* arena,
                         std::uint32_t* alloc_count) noexcept;

}  // namespace sotc
