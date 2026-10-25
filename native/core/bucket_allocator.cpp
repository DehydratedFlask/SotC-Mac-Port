#include "bucket_allocator.hpp"

namespace sotc {

BucketAllocator::BucketAllocator(PageOwnerMap& owners, std::uint32_t owner_id)
    : owners_(owners), owner_id_(owner_id) {}

BucketAllocator::~BucketAllocator() {
    std::vector<HostMapping> pages;
    std::unordered_map<void*, std::uint32_t> live;
    {
        std::lock_guard<std::mutex> lock(mu_);
        pages.swap(pages_);
        live.swap(live_);
        for (auto& fl : free_) fl.clear();
    }
    for (const auto& kv : live) owners_.remove(reinterpret_cast<std::uintptr_t>(kv.first));
    for (HostMapping& m : pages) unmap(m);
}

bool BucketAllocator::refill_class(std::size_t index, std::uint32_t class_bytes) {
    auto mapped = map_anonymous_aligned(kBucketPageBytes, kBucketPageBytes);
    if (!mapped) return false;

    auto* base = static_cast<unsigned char*>(mapped->ptr);
    // Preserve the 0x38-byte page header, then align the first slot to its class stride.
    // Every slot stays class-aligned because the class is a multiple of each request alignment.
    auto* cursor = base + kBucketPageHeader;
    const auto first_address = reinterpret_cast<std::uintptr_t>(cursor);
    const std::uintptr_t remainder = first_address % class_bytes;
    if (remainder != 0) cursor += class_bytes - remainder;
    const auto used = static_cast<std::size_t>(cursor - base);
    if (used >= kBucketPageBytes) {
        unmap(*mapped);
        return false;
    }
    const std::uint32_t slots = static_cast<std::uint32_t>((kBucketPageBytes - used) / class_bytes);
    if (slots == 0) {
        unmap(*mapped);
        return false;
    }
    for (std::uint32_t i = 0; i < slots; ++i) {
        free_[index].push_back(cursor);
        cursor += class_bytes;
    }
    pages_.push_back(*mapped);
    ++stats_.pages_mapped;
    return true;
}

void* BucketAllocator::allocate(std::size_t size, std::size_t align, const AllocDesc&) {
    const auto cls = bucket_class_bytes(size, align);
    if (!cls) return nullptr;
    const std::uint32_t class_bytes = *cls;
    const std::size_t index = bucket_class_index(class_bytes);
    if (index >= kBucketClassCount) return nullptr;

    std::lock_guard<std::mutex> lock(mu_);
    if (free_[index].empty()) {
        if (!refill_class(index, class_bytes)) return nullptr;
    }
    void* p = free_[index].back();
    free_[index].pop_back();

    if (owners_.add(reinterpret_cast<std::uintptr_t>(p), class_bytes, owner_id_) !=
        PageOwnerMap::Status::ok) {
        free_[index].push_back(p);
        return nullptr;
    }
    live_[p] = class_bytes;
    ++stats_.live_blocks;
    stats_.bytes_in_use += class_bytes;
    if (stats_.bytes_in_use > stats_.peak_bytes) stats_.peak_bytes = stats_.bytes_in_use;
    // OBS: no zero-fill — leave page contents as returned by the host map.
    return p;
}

void BucketAllocator::deallocate(void* ptr, const AllocDesc&) {
    if (ptr == nullptr) return;
    std::uint32_t class_bytes = 0;
    {
        std::lock_guard<std::mutex> lock(mu_);
        auto it = live_.find(ptr);
        if (it == live_.end()) {
            ++stats_.ignored_frees;
            return;
        }
        class_bytes = it->second;
        live_.erase(it);
        --stats_.live_blocks;
        stats_.bytes_in_use -= class_bytes;
        const std::size_t index = bucket_class_index(class_bytes);
        free_[index].push_back(ptr);
    }
    owners_.remove(reinterpret_cast<std::uintptr_t>(ptr));
}

bool BucketAllocator::owns_block(const void* p) const {
    std::lock_guard<std::mutex> lock(mu_);
    return live_.count(const_cast<void*>(p)) != 0;  // map keyed by void*; lookup only
}

BucketAllocator::Stats BucketAllocator::stats() const {
    std::lock_guard<std::mutex> lock(mu_);
    return stats_;
}

}  // namespace sotc
