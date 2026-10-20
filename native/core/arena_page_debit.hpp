// SRV-01: arena page-count debit + idle-pool insert helpers (OBS free path).
// Page debit: 0x1ae67..0x1ae6b (also 0x1abd2 uses same [arena+0x70] cell).
// Idle insert: 0x1aecd..0x1af03 — same surgery as list_insert_at_head on global pool.
// Not page-map tables @0x1ab08; not 0x19660.
#pragma once

#include <cstdint>

#include "region_list.hpp"

namespace sotc {

// OBS: dword page counter at arena +0x70 (carve budget uses >>16 pages).
inline constexpr std::size_t kArenaPageCountOffset = 0x70;

inline std::uint32_t& arena_page_count(void* arena) noexcept {
    return *reinterpret_cast<std::uint32_t*>(static_cast<char*>(arena) + kArenaPageCountOffset);
}

// 0x1ae67..0x1ae6b: pages = bytes >> 16; sub dword [arena+0x70], pages.
void arena_debit_pages(void* arena, std::uint64_t bytes) noexcept;

// Idle / freelist pool OBS layout relative to [0x2dfca78] (runtime global; not resolved here):
//   +0x1e0 embedded sentinel (owner/prev target)
//   +0x1f0 head next Links*
//   +0x200 node count dword
inline constexpr std::size_t kIdlePoolSentinelOffset = 0x1e0;
inline constexpr std::size_t kIdlePoolHeadOffset = 0x1f0;
inline constexpr std::size_t kIdlePoolCountOffset = 0x200;

inline void* idle_pool_sentinel(void* pool) noexcept {
    return static_cast<char*>(pool) + kIdlePoolSentinelOffset;
}
inline IntrusiveLinks** idle_pool_head_slot(void* pool) noexcept {
    return reinterpret_cast<IntrusiveLinks**>(static_cast<char*>(pool) + kIdlePoolHeadOffset);
}
inline std::uint32_t& idle_pool_count(void* pool) noexcept {
    return *reinterpret_cast<std::uint32_t*>(static_cast<char*>(pool) + kIdlePoolCountOffset);
}

// OBS 0x1aed4..0x1af03: insert node at idle-pool head (reuses list_insert_at_head).
inline void idle_pool_insert(RegionNode& node, void* pool) noexcept {
    list_insert_at_head(node, idle_pool_head_slot(pool), idle_pool_sentinel(pool),
                        &idle_pool_count(pool));
}

}  // namespace sotc
