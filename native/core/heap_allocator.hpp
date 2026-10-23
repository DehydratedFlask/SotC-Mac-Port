// SRV-01: HeapAllocator layer (object 0x2dfc700; usable-size vt+0x30 = 0x48c70;
// allocate vt+0xa0 = 0x48c80; deallocate vt+0xa8 = 0x493d0; page-size setter 0x488f0;
// arena reserve / "cap" 0x48910 → 0x479a0).
//
// OBS (private contract + tools/lift/check_f48c70.py):
//   * Bootstrap: 0x488f0 rounds esi up to 64 KiB and stores at [obj+0xd0] (1 MiB page quantum).
//   * Bootstrap: 0x48910(obj, 0x9600000) maps that many bytes via 0x479a0 with align 0x10000 and
//     installs the region as the free-list arena (root at +0xb8). So the "cap" is an arena
//     reservation size, not only a soft counter.
//   * Composite routes size < 0x100000 here; allocate path 16-byte-aligns the request
//     (`addq $0xf; andq $-0x10`) and may replenish via 0x479a0.
//   * Usable size helper 0x48c70: load qword at ptr-8, mask ~0x7 (low 3 bits are flags).
//   * No zero-fill in decoded paths.
// INF (native design):
//   * Chunk freelist / 0x49030 / 0x48a70 surgery is NOT lifted. Native maps one host block per
//     allocate (header + payload), enforces live bytes <= cap, rejects size > page quantum.
//   * Header is 8 bytes before the returned pointer (matches 0x48c70); PageOwnerMap registers
//     the user pointer.
#pragma once

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <unordered_map>

#include "composite_allocator.hpp"
#include "host_map.hpp"
#include "iallocator.hpp"
#include "page_owner_map.hpp"

namespace sotc {

// OBS 0x48c80: (size + 0xf) & ~0xf — and align bump. nullopt if above the heap page quantum.
inline std::optional<std::uint64_t> heap_request_bytes(std::size_t size, std::size_t align) noexcept {
    std::size_t need = size == 0 ? 1 : size;
    if (align > need) need = align;
    const std::uint64_t rounded =
        (static_cast<std::uint64_t>(need) + 0xfu) & ~std::uint64_t{0xfu};
    if (rounded == 0 || rounded > kHeapPageBytes) return std::nullopt;
    return rounded;
}

// OBS 0x488f0: (page + 0xffff) & ~0xffff — 64 KiB round-up used before storing +0xd0.
inline std::uint64_t heap_round_page_quantum(std::uint64_t page_bytes) noexcept {
    return (page_bytes + 0xffffu) & ~std::uint64_t{0xffffu};
}

// OBS 0x48c70: usable payload size from header at user_ptr - 8.
inline std::uint64_t heap_usable_size(const void* user_ptr) noexcept {
    if (!user_ptr) return 0;
    const auto* hdr =
        reinterpret_cast<const std::uint64_t*>(static_cast<const unsigned char*>(user_ptr) - 8);
    return *hdr & ~std::uint64_t{7};
}

class HeapAllocator final : public IAllocator {
public:
    // `cap_bytes` mirrors the 0x48910 reservation argument (bootstrap 0x9600000).
    // `page_bytes` mirrors the 0x488f0 argument (bootstrap 0x100000) after 64 KiB round-up.
    HeapAllocator(PageOwnerMap& owners, std::uint32_t owner_id,
                  std::uint64_t cap_bytes = kHeapAllocatorCapBytes,
                  std::uint64_t page_bytes = kHeapPageBytes);
    ~HeapAllocator() override;
    HeapAllocator(const HeapAllocator&) = delete;
    HeapAllocator& operator=(const HeapAllocator&) = delete;

    void* allocate(std::size_t size, std::size_t align, const AllocDesc& desc) override;
    void deallocate(void* ptr, const AllocDesc& desc) override;

    bool owns_block(const void* p) const;
    std::uint64_t cap_bytes() const noexcept { return cap_bytes_; }
    std::uint64_t page_bytes() const noexcept { return page_bytes_; }

    struct Stats {
        std::uint64_t live_blocks = 0;
        std::uint64_t bytes_in_use = 0;  // sum of payload sizes
        std::uint64_t peak_bytes = 0;
        std::uint64_t ignored_frees = 0;
        std::uint64_t cap_rejects = 0;
    };
    Stats stats() const;

private:
    struct Block {
        HostMapping mapping;
        std::uint64_t payload;  // usable size stored in header
    };

    PageOwnerMap& owners_;
    std::uint32_t owner_id_;
    std::uint64_t cap_bytes_;
    std::uint64_t page_bytes_;
    mutable std::mutex mu_;
    std::unordered_map<void*, Block> live_;
    Stats stats_;
};

}  // namespace sotc
