#include "heap_allocator.hpp"

#include <cstring>
#include <limits>

namespace sotc {
namespace {

constexpr std::size_t kHeader = 8;

}  // namespace

HeapAllocator::HeapAllocator(PageOwnerMap& owners, std::uint32_t owner_id, std::uint64_t cap_bytes,
                             std::uint64_t page_bytes)
    : owners_(owners),
      owner_id_(owner_id),
      cap_bytes_(cap_bytes),
      page_bytes_(heap_round_page_quantum(page_bytes == 0 ? kHeapPageBytes : page_bytes)) {}

HeapAllocator::~HeapAllocator() {
    std::unordered_map<void*, Block> live;
    {
        std::lock_guard<std::mutex> lock(mu_);
        live.swap(live_);
    }
    for (auto& kv : live) {
        owners_.remove(reinterpret_cast<std::uintptr_t>(kv.first));
        unmap(kv.second.mapping);
    }
}

void* HeapAllocator::allocate(std::size_t size, std::size_t align, const AllocDesc&) {
    const auto payload = heap_request_bytes(size, align);
    if (!payload) return nullptr;
    if (align > std::numeric_limits<std::uint32_t>::max()) return nullptr;

    // Map header + payload; host alignment at least 16 / request align (INF; original carves).
    const std::uint64_t map_size = *payload + kHeader;
    std::uint32_t map_align = 16;
    if (align > map_align) map_align = static_cast<std::uint32_t>(align);

    std::lock_guard<std::mutex> lock(mu_);
    if (stats_.bytes_in_use > cap_bytes_ || *payload > cap_bytes_ - stats_.bytes_in_use) {
        ++stats_.cap_rejects;
        return nullptr;
    }

    auto mapped = map_anonymous_aligned(map_size, map_align);
    if (!mapped) return nullptr;

    auto* raw = static_cast<unsigned char*>(mapped->ptr);
    auto* hdr = reinterpret_cast<std::uint64_t*>(raw);
    *hdr = *payload;  // OBS low 3 bits clear (flags unused here)
    void* user = raw + kHeader;

    if (owners_.add(reinterpret_cast<std::uintptr_t>(user), static_cast<std::size_t>(*payload),
                    owner_id_) != PageOwnerMap::Status::ok) {
        unmap(*mapped);
        return nullptr;
    }
    live_.emplace(user, Block{*mapped, *payload});
    ++stats_.live_blocks;
    stats_.bytes_in_use += *payload;
    if (stats_.bytes_in_use > stats_.peak_bytes) stats_.peak_bytes = stats_.bytes_in_use;
    return user;
}

void HeapAllocator::deallocate(void* ptr, const AllocDesc&) {
    if (ptr == nullptr) return;
    Block blk{};
    {
        std::lock_guard<std::mutex> lock(mu_);
        auto it = live_.find(ptr);
        if (it == live_.end()) {
            ++stats_.ignored_frees;
            return;
        }
        blk = it->second;
        live_.erase(it);
        --stats_.live_blocks;
        stats_.bytes_in_use -= blk.payload;
    }
    owners_.remove(reinterpret_cast<std::uintptr_t>(ptr));
    unmap(blk.mapping);
}

bool HeapAllocator::owns_block(const void* p) const {
    std::lock_guard<std::mutex> lock(mu_);
    return live_.count(const_cast<void*>(p)) != 0;
}

HeapAllocator::Stats HeapAllocator::stats() const {
    std::lock_guard<std::mutex> lock(mu_);
    return stats_;
}

}  // namespace sotc
