#include "page_owner_map.hpp"

namespace sotc {

PageOwnerMap::Status PageOwnerMap::add(std::uintptr_t start, std::size_t size,
                                       std::uint32_t owner_id) {
    if (owner_id < kMinOwnerId || owner_id > kMaxOwnerId) return Status::bad_owner_id;
    if (size == 0 || start + size < start) return Status::empty_or_wrapping_range;
    const std::uintptr_t end = start + size;
    std::lock_guard<std::mutex> lock(mu_);
    // First range whose start is >= `start`; it must begin at or after `end`.
    auto next = ranges_.lower_bound(start);
    if (next != ranges_.end() && next->first < end) return Status::overlaps_existing;
    if (next != ranges_.begin()) {
        auto prev = std::prev(next);
        if (prev->second.end > start) return Status::overlaps_existing;
    }
    ranges_.emplace(start, Entry{end, owner_id});
    return Status::ok;
}

PageOwnerMap::Status PageOwnerMap::remove(std::uintptr_t start) {
    std::lock_guard<std::mutex> lock(mu_);
    return ranges_.erase(start) == 1 ? Status::ok : Status::not_found;
}

std::optional<std::uint32_t> PageOwnerMap::lookup(const void* p) const {
    if (p == nullptr) return std::nullopt;
    const auto a = reinterpret_cast<std::uintptr_t>(p);
    std::lock_guard<std::mutex> lock(mu_);
    auto it = ranges_.upper_bound(a);
    if (it == ranges_.begin()) return std::nullopt;
    --it;
    if (a >= it->second.end) return std::nullopt;
    if (it->second.owner < kMinOwnerId) return std::nullopt;  // OBS id < 2 => no owner
    return it->second.owner;
}

std::size_t PageOwnerMap::range_count() const {
    std::lock_guard<std::mutex> lock(mu_);
    return ranges_.size();
}

}  // namespace sotc
