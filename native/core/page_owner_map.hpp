// SRV-01 allocator front-end support: pointer -> owning-heap-id lookup.
//
// Native analogue of the original page-table lookup at RVA 0x19a50 (OBS: reads an id byte per
// 64 KiB / 1 MiB page; id < 2 => "no owner"; unknown pointers => no owner). The original byte
// table layout is NOT reproduced: this is an ordered map of registered half-open ranges
// (INF design choice). Contract-derived behaviour:
//   OBS  ids 0 and 1 never resolve to an owner (0x19a50 returns NULL for id < 2).
//   OBS  a pointer outside every registered range has no owner (free of a foreign pointer is
//        silently ignored by the caller).
//   INF  lookup is by containing range (any interior pointer resolves), thread-safe.
#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <map>
#include <mutex>
#include <optional>

namespace sotc {

class PageOwnerMap {
public:
    // Owner ids are the heap-table slot (0..255). 0 and 1 are reserved (never owners).
    static constexpr std::uint32_t kMinOwnerId = 2;
    static constexpr std::uint32_t kMaxOwnerId = 255;

    enum class Status { ok, bad_owner_id, empty_or_wrapping_range, overlaps_existing, not_found };

    // Registers [start, start+size) for `owner_id`. Fails (no change) on reserved id, size 0,
    // address wrap, or overlap with an already registered range.
    Status add(std::uintptr_t start, std::size_t size, std::uint32_t owner_id);

    // Removes the range that begins exactly at `start`.
    Status remove(std::uintptr_t start);

    // Owner of the range containing `p`, or nullopt (null pointer, foreign pointer).
    std::optional<std::uint32_t> lookup(const void* p) const;

    std::size_t range_count() const;

private:
    struct Entry {
        std::uintptr_t end;
        std::uint32_t owner;
    };
    mutable std::mutex mu_;
    std::map<std::uintptr_t, Entry> ranges_;
};

}  // namespace sotc
