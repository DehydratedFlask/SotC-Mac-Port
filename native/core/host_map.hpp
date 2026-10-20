// SRV-01: Darwin host mapping behind allocator region API 0x19ae0 / 0x19e00.
// Replaces sceKernelReserveVirtualRange + sceKernelMapFlexibleMemory for the
// preferred_base==0 path used by the Shadow-heap call site. Not a lift of the
// internal free-list carver at 0x1a800. See docs/POINTERS.md and
// tools/lift/check_host_map_calls.py.
#pragma once

#include <cstdint>
#include <optional>

#include "allocator_region.hpp"

namespace sotc {

// One anonymous RW mapping. `ptr`/`size` are the aligned usable range;
// `raw`/`raw_size` are the span that must be munmap'd together (same as ptr/size
// after edge trim).
struct HostMapping {
    void* ptr = nullptr;
    std::uint64_t size = 0;  // page-rounded mapped length
    void* raw = nullptr;
    std::uint64_t raw_size = 0;

    bool empty() const noexcept { return ptr == nullptr || size == 0; }
};

// Successful map_region: live pages + the R{size,start} view the game records.
struct MappedRegion {
    HostMapping mapping;
    AllocatorRegion region;
};

// Alignment applied before ReserveVirtual at 0x19fb2: ceil to 64 KiB (0x19e2d),
// then ceil to 1 MiB (0x19f0d), then max(align, 1 MiB) (0x19f8a..0x19fa9).
inline std::uint32_t reserve_alignment(std::uint32_t alignment) noexcept {
    std::uint32_t a = align_up_64k(alignment);
    a = align_up_1m(a);
    return a < 0x100000u ? 0x100000u : a;
}

// Map anonymous RW pages with `size` bytes and `alignment` (rounded via
// reserve_alignment). Fails on zero size, non-power-of-two align after round,
// overflow, or mmap failure.
std::optional<HostMapping> map_anonymous_aligned(std::uint64_t size,
                                                 std::uint32_t alignment) noexcept;

void unmap(HostMapping& mapping) noexcept;

// Fulfill an AllocatorRegionRequest via host mmap when preferred_base==0.
// region.size = req.size (OBS store at 0x19fd9); region.start = mapped VA.
// Nonzero preferred_base → nullopt (MAP_FIXED policy not adopted this slice).
std::optional<MappedRegion> map_region(const AllocatorRegionRequest& req) noexcept;

}  // namespace sotc
