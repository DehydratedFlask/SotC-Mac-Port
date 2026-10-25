#include "heap_allocator.hpp"

#include <cstring>
#include <limits>

namespace sotc {
namespace {

constexpr std::size_t kHeader = 8;

}  // namespace

HeapAllocator::HeapAllocator(PageOwnerMap& owners, std::uint32_t owner_id,
                             std::uint64_t cap_bytes)
    : owners_(owners), owner_id_(owner_id), cap_bytes_(cap_bytes) {}

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

    // Leave enough room to align the user pointer after its 8-byte size header.
    const std::uint64_t alignment = align == 0 ? 1 : align;
    const std::uint64_t padding = alignment - 1;
    if (*payload > std::numeric_limits<std::uint64_t>::max() - kHeader - padding) return nullptr;
    const std::uint64_t map_size = kHeader + padding + *payload;
    const std::uint32_t map_align = 16;

    std::lock_guard<std::mutex> lock(mu_);
    if (stats_.bytes_in_use > cap_bytes_ || *payload > cap_bytes_ - stats_.bytes_in_use) {
        ++stats_.cap_rejects;
        return nullptr;
    }

    auto mapped = map_anonymous_aligned(map_size, map_align);
    if (!mapped) return nullptr;

    auto* raw = static_cast<unsigned char*>(mapped->ptr);
    const auto after_header = reinterpret_cast<std::uintptr_t>(raw + kHeader);
    const auto remainder = after_header % alignment;
    const auto user_padding = remainder == 0 ? 0 : alignment - remainder;
    auto* user_bytes = raw + kHeader + user_padding;
    std::memcpy(user_bytes - kHeader, &*payload, sizeof(*payload));  // OBS low 3 bits clear
    void* user = user_bytes;

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
