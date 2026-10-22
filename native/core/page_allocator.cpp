#include "page_allocator.hpp"

#include <limits>

namespace sotc {

std::optional<CtxMappedRegion> map_region_with_ctx(const CtxRegionRequest& req) noexcept {
    if (req.size == 0) return std::nullopt;
    const std::uint64_t rounded = align_size_up_64k(req.size);
    if (rounded < req.size) return std::nullopt;  // 64-bit wrap in the rounding

    AllocatorRegionRequest host_req;
    host_req.preferred_base = 0;  // ctx is NOT a preferred VA (see header)
    host_req.size = rounded;
    host_req.alignment = req.alignment;
    host_req.flags = req.flags;

    auto mapped = map_region(host_req);
    if (!mapped) return std::nullopt;
    CtxMappedRegion out;
    out.mapped = *mapped;
    out.ctx = req.ctx;
    return out;
}

PageAllocator::PageAllocator(PageOwnerMap& owners, std::uint32_t owner_id,
                             std::uint32_t region_flags)
    : owners_(owners), owner_id_(owner_id), region_flags_(region_flags) {}

PageAllocator::~PageAllocator() {
    std::map<std::uintptr_t, Block> leftover;
    {
        std::lock_guard<std::mutex> lock(mu_);
        leftover.swap(blocks_);
    }
    for (auto& kv : leftover) {
        owners_.remove(kv.first);
        unmap(kv.second.mapping);
    }
}

void* PageAllocator::allocate(std::size_t size, std::size_t align, const AllocDesc&) {
    if (size == 0) size = 1;  // defensive: the front-end already maps 0 -> 1
    if (align > std::numeric_limits<std::uint32_t>::max()) return nullptr;

    CtxRegionRequest req;
    req.ctx = reinterpret_cast<std::uintptr_t>(this);
    req.size = size;
    req.alignment = static_cast<std::uint32_t>(align);
    req.flags = region_flags_;
    auto r = map_region_with_ctx(req);
    if (!r) return nullptr;

    const std::uintptr_t start = static_cast<std::uintptr_t>(r->mapped.region.start);
    const std::uint64_t rsize = r->mapped.region.size;
    if (owners_.add(start, static_cast<std::size_t>(rsize), owner_id_) !=
        PageOwnerMap::Status::ok) {
        unmap(r->mapped.mapping);
        return nullptr;
    }
    {
        std::lock_guard<std::mutex> lock(mu_);
        blocks_.emplace(start, Block{r->mapped.mapping, rsize});
        ++stats_.live_blocks;
        stats_.bytes_in_use += rsize;
        if (stats_.bytes_in_use > stats_.peak_bytes) stats_.peak_bytes = stats_.bytes_in_use;
    }
    return reinterpret_cast<void*>(start);
}

void PageAllocator::deallocate(void* ptr, const AllocDesc&) {
    if (ptr == nullptr) return;
    const auto key = reinterpret_cast<std::uintptr_t>(ptr);
    Block blk{};
    {
        std::lock_guard<std::mutex> lock(mu_);
        auto it = blocks_.find(key);
        if (it == blocks_.end()) {
            ++stats_.ignored_frees;
            return;
        }
        blk = it->second;
        blocks_.erase(it);
        --stats_.live_blocks;
        stats_.bytes_in_use -= blk.size;
    }
    owners_.remove(key);  // unregister before the VA can be reused by a later mapping
    unmap(blk.mapping);
}

bool PageAllocator::owns_block(const void* p) const {
    std::lock_guard<std::mutex> lock(mu_);
    return blocks_.count(reinterpret_cast<std::uintptr_t>(p)) != 0;
}

PageAllocator::Stats PageAllocator::stats() const {
    std::lock_guard<std::mutex> lock(mu_);
    return stats_;
}

}  // namespace sotc
