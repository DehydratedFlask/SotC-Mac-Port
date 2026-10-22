// SRV-01: page-allocator layer (original PageAllocator object 0x2dfc800, wrapper 0x479a0 ->
// region layer 0x19ae0), implemented over host_map.
//
// OBS: size (+tracker extra) is rounded up to 64 KiB (0x479a0: add 0xffff; and -0x10000), then
//      0x19ae0(ctx = this heap object, size, align, flags = [this+0x18]); the returned pointer is
//      the region start (no header before it); the region records ctx so the page table can map
//      ptr -> owner id (0x19ff6/0x1aafd write ctx[+0x1c]); mappings use >= 1 MiB alignment.
// OBS (gap): host_map::map_region rejects a nonzero AllocatorRegionRequest::preferred_base because
//      the original rdi at this call site is an object context, not a preferred VA.
// INF: map_region_with_ctx below is the native resolution: it keeps `ctx` out of band, asks
//      host_map with preferred_base == 0 and returns ctx with the mapping. The tracker "extra" size
//      (profiling) is not modelled. Blocks are unmapped on free and on destruction (native hygiene;
//      the original never unmaps at shutdown). Free of a non-block-start pointer is ignored.
#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <map>
#include <mutex>
#include <optional>

#include "host_map.hpp"
#include "iallocator.hpp"
#include "page_owner_map.hpp"

namespace sotc {

struct CtxRegionRequest {
    std::uint64_t ctx = 0;        // OBS rdi at 0x479c2: owning heap object (not a VA)
    std::uint64_t size = 0;       // raw size; rounded to 64 KiB by the wrapper
    std::uint32_t alignment = 0;  // OBS edx
    std::uint32_t flags = 0;      // OBS ecx = [heap+0x18]
};

struct CtxMappedRegion {
    MappedRegion mapped;  // mapped.region.size == rounded size, .start == block pointer
    std::uint64_t ctx = 0;
};

// Round size to 64 KiB, build the preferred_base==0 request, map, tag with ctx. nullopt on
// size 0, rounding overflow, alignment > UINT32_MAX handled by caller, or host failure.
std::optional<CtxMappedRegion> map_region_with_ctx(const CtxRegionRequest& req) noexcept;

class PageAllocator final : public IAllocator {
public:
    // `owners` must outlive this object; `owner_id` in [2,255] is the heap-table slot this
    // allocator is installed at (used as the page-owner id); `region_flags` is [heap+0x18].
    PageAllocator(PageOwnerMap& owners, std::uint32_t owner_id, std::uint32_t region_flags = 0);
    ~PageAllocator() override;
    PageAllocator(const PageAllocator&) = delete;
    PageAllocator& operator=(const PageAllocator&) = delete;

    void* allocate(std::size_t size, std::size_t align, const AllocDesc& desc) override;
    void deallocate(void* ptr, const AllocDesc& desc) override;

    // Owner lookup for free: true when `p` is the start of a live block of this allocator.
    bool owns_block(const void* p) const;

    struct Stats {
        std::uint64_t live_blocks = 0;
        std::uint64_t bytes_in_use = 0;   // rounded sizes
        std::uint64_t peak_bytes = 0;
        std::uint64_t ignored_frees = 0;  // deallocate() on a non-block-start pointer
    };
    Stats stats() const;

private:
    struct Block {
        HostMapping mapping;
        std::uint64_t size;  // rounded
    };
    PageOwnerMap& owners_;
    std::uint32_t owner_id_;
    std::uint32_t region_flags_;
    mutable std::mutex mu_;
    std::map<std::uintptr_t, Block> blocks_;
    Stats stats_;
};

}  // namespace sotc
