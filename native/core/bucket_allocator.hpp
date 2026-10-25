// SRV-01: BucketAllocator layer (object 0x2dfb600; init 0x17990; pool setup 0x18750;
// allocate entry 0x179c0). Contract + ELF constants — not a byte-for-byte freelist lift.
//
// OBS (private contract + tools/lift/check_f17990.py):
//   * Size band for this heap: requests with size <= [obj+0x88] (bootstrap max 0x400).
//     Composite routes only size < 0x400 here; size == 0x400 is accepted if called directly.
//   * 64 size classes: 0x10..0x400 step 0x10 (bootstrap pool loop rbx 0x10..0x410, stride 0x40).
//   * Backing pages are 64 KiB ([obj+0xa8] = 0x10000); pool setup reserves 0x38 header bytes
//     then divides the remainder by the class size for slot count.
//   * No zero-fill in decoded allocate paths.
// INF (native design):
//   * Freelist is a simple per-class stack of pointers (original uses richer page/chunk lists
//     at 0x17d20 / 0x179c0 — not reproduced).
//   * Native class selection rounds the request to a size-class stride divisible by both the
//     request size and alignment; classes remain multiples of 0x10 and <= 0x400, else nullptr.
//   * Each live block is registered in PageOwnerMap (analogue of page-table owner id).
//   * Not modelled: spinlocks, tracker hooks, exact chunk metadata layout.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <mutex>
#include <numeric>
#include <optional>
#include <unordered_map>
#include <vector>

#include "composite_allocator.hpp"  // kBucket* / kCompositeBucketThreshold constants
#include "host_map.hpp"
#include "iallocator.hpp"
#include "page_owner_map.hpp"

namespace sotc {

inline constexpr std::size_t kBucketClassCount = 64;       // OBS (0x400-0x10)/0x10+1
inline constexpr std::uint32_t kBucketPageHeader = 0x38;  // OBS 0x18750: add page,-0x38 before div

// Round a request up to the owning size class. nullopt when it cannot fit in the bucket band.
inline std::optional<std::uint32_t> bucket_class_bytes(std::size_t size, std::size_t align) noexcept {
    std::size_t need = size == 0 ? 1 : size;
    const std::size_t alignment = align == 0 ? 1 : align;
    if (alignment > need) need = alignment;

    // A pool's stride must be divisible by every supported alignment assigned to it.
    // Round to lcm(class step, requested alignment), rejecting arithmetic overflow.
    const std::size_t divisor = std::gcd(std::size_t{kBucketClassStep}, alignment);
    const std::size_t factor = std::size_t{kBucketClassStep} / divisor;
    if (factor > std::numeric_limits<std::size_t>::max() / alignment) return std::nullopt;
    const std::size_t quantum = factor * alignment;
    if (need > std::numeric_limits<std::size_t>::max() - (quantum - 1)) return std::nullopt;
    const std::size_t cls = ((need + quantum - 1) / quantum) * quantum;
    if (cls > kCompositeBucketThreshold) return std::nullopt;  // OBS max at [obj+0x88]
    return static_cast<std::uint32_t>(cls);
}

inline std::size_t bucket_class_index(std::uint32_t class_bytes) noexcept {
    return (static_cast<std::size_t>(class_bytes) / kBucketClassStep) - 1;
}

// OBS 0x18750 upper-bound slot count before native alignment padding.
inline std::uint32_t bucket_slots_per_page(std::uint32_t class_bytes,
                                          std::uint32_t page_bytes = kBucketPageBytes) noexcept {
    if (class_bytes == 0 || page_bytes <= kBucketPageHeader) return 0;
    return (page_bytes - kBucketPageHeader) / class_bytes;
}

class BucketAllocator final : public IAllocator {
public:
    BucketAllocator(PageOwnerMap& owners, std::uint32_t owner_id);
    ~BucketAllocator() override;
    BucketAllocator(const BucketAllocator&) = delete;
    BucketAllocator& operator=(const BucketAllocator&) = delete;

    void* allocate(std::size_t size, std::size_t align, const AllocDesc& desc) override;
    void deallocate(void* ptr, const AllocDesc& desc) override;

    bool owns_block(const void* p) const;

    struct Stats {
        std::uint64_t live_blocks = 0;
        std::uint64_t bytes_in_use = 0;  // sum of class sizes of live blocks
        std::uint64_t peak_bytes = 0;
        std::uint64_t pages_mapped = 0;
        std::uint64_t ignored_frees = 0;
    };
    Stats stats() const;

private:
    bool refill_class(std::size_t index, std::uint32_t class_bytes);

    PageOwnerMap& owners_;
    std::uint32_t owner_id_;
    mutable std::mutex mu_;
    std::array<std::vector<void*>, kBucketClassCount> free_;
    std::unordered_map<void*, std::uint32_t> live_;  // ptr -> class_bytes
    std::vector<HostMapping> pages_;
    Stats stats_;
};

}  // namespace sotc
