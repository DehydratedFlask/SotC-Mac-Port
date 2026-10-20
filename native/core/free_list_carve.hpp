// SRV-01: pure free-list carve-fit math from original RVA 0x1a800 (fit/align only).
// Not a lift of node alloc 0x19660, list surgery, or page-bitmap updates.
// Callers: 0x19e72 / 0x19f47 inside 0x19e00. See tools/lift/check_f1a800_carve.py.
#pragma once

#include <cstdint>
#include <optional>

namespace sotc {

// Result of trying to place req_size into a free region [free_start, free_start+free_size)
// with power-of-two alignment, matching OBS at 0x1a866..0x1a888 / 0x1a8a1.
struct CarveFit {
    std::uint64_t lead = 0;             // front pad kept as free (0 → consume whole node)
    std::uint64_t allocated_start = 0;  // aligned start handed to the allocation
    std::uint64_t allocated_size = 0;   // OBS: free_size - lead (not trimmed to req_size)
    bool exact_node = false;            // lead==0 path at 0x1a8a1
};

// Arena page-budget gate at 0x1a81c..0x1a824: need (req_size >> 16) pages.
inline bool carve_pages_available(std::uint32_t arena_page_count,
                                  std::uint64_t req_size) noexcept {
    return arena_page_count >= static_cast<std::uint32_t>(req_size >> 16);
}

// Alignment prep at 0x1a83d..0x1a842: mask = ~(alignment - 1). alignment must be
// a non-zero power of two (as produced by prior 64KiB/1MiB rounding in 0x19e00).
inline std::uint64_t carve_align_mask(std::uint32_t alignment) noexcept {
    return ~static_cast<std::uint64_t>(alignment - 1u);
}

// Try one free region. Returns nullopt when free_size < req_size, alignment invalid,
// or lead + req_size does not fit (0x1a881..0x1a888).
std::optional<CarveFit> try_carve_fit(std::uint64_t free_start, std::uint64_t free_size,
                                      std::uint64_t req_size,
                                      std::uint32_t alignment) noexcept;

}  // namespace sotc
