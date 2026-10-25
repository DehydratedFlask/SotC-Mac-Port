#include "composite_allocator.hpp"

namespace sotc {

bool CompositeAllocator::add_child(std::uint64_t threshold, IAllocator* heap) noexcept {
    if (heap == nullptr || children_.size() >= kMaxChildren) return false;
    children_.push_back(CompositeChild{threshold, heap});
    return true;
}

void* CompositeAllocator::allocate(std::size_t size, std::size_t align, const AllocDesc& desc) {
    // OBS 0x47ed0: cmp size, threshold; jbe skip — so only size < threshold is tried.
    for (const CompositeChild& child : children_) {
        if (size >= child.threshold) continue;
        void* p = child.heap->allocate(size, align, desc);
        if (p == nullptr) continue;  // OBS: try next child when this layer fails
        {
            std::lock_guard<std::mutex> lock(mu_);
            live_[p] = child.heap;
        }
        bytes_dispatched_.fetch_add(size, std::memory_order_relaxed);  // OBS addq size, [+0xd0]
        return p;
    }
    return nullptr;
}

void CompositeAllocator::deallocate(void* ptr, const AllocDesc& desc) {
    if (ptr == nullptr) return;
    IAllocator* child = nullptr;
    {
        std::lock_guard<std::mutex> lock(mu_);
        auto it = live_.find(ptr);
        if (it == live_.end()) return;  // INF: foreign / already freed — ignore (matches free ignore)
        child = it->second;
        live_.erase(it);
    }
    child->deallocate(ptr, desc);
}

void CompositeAllocator::on_owner_map_deallocation(void* ptr) noexcept {
    if (ptr == nullptr) return;
    IAllocator* child = nullptr;
    {
        std::lock_guard<std::mutex> lock(mu_);
        auto it = live_.find(ptr);
        if (it != live_.end()) {
            child = it->second;
            live_.erase(it);
        }
    }
    if (child != nullptr) child->on_owner_map_deallocation(ptr);
}

std::size_t CompositeAllocator::tracked_allocation_count() const {
    std::lock_guard<std::mutex> lock(mu_);
    return live_.size();
}

}  // namespace sotc
