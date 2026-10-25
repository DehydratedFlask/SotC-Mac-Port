// SRV-01: minimal heap interface for the allocator front-end (0x12820 / 0x12af0).
//
// The original heaps are bpe::memory::IAllocator objects (OBS: Memory.cpp strings); the front-end
// reaches them only through three virtual slots, mirrored here:
//   OBS  vt[+0xa0] allocate(heap, size, align, &desc) -> ptr (NULL on failure)
//   OBS  vt[+0x98] "recover" query called after a failed allocate: returns bool; when true the
//                  front-end skips OOM reporting and returns NULL  (all decoded children use a
//                  stub returning 0, so in practice reporting always runs)
//   OBS  vt[+0xa8] deallocate(heap, ptr, &desc{kind})
// The names, the C++ signature and the 64-bit size/align types are INF (native design).
// PageAllocator, CompositeAllocator, BucketAllocator, and HeapAllocator implement this;
// HeapAllocator freelist/arena surgery is still INF (cap + page quantum are OBS).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

// Descriptor passed to the heap. OBS: 32 bytes on the original stack with a u32 tag (global
// [0x2dd9fa8]) and a u32 flags word; consumed only by a profiling tracker. Native keeps the two
// words and treats both as opaque pass-through.
struct AllocDesc {
    std::uint32_t flags = 0;  // OBS call-site values: 1 = new, 2 = new[]; kind 3 seen at 0x12950
    std::uint32_t tag = 0;    // opaque (OBS: global context tag at the time of the call)
};

inline constexpr std::uint32_t kAllocFlagNew = 1;       // OBS wrappers 0x17070 / free kind of 0x170b0
inline constexpr std::uint32_t kAllocFlagNewArray = 2;  // OBS wrappers 0x17090 / free kind of 0x170c0

class IAllocator {
public:
    virtual ~IAllocator() = default;

    // size >= 1 (the front-end maps 0 to 1). align is passed through unmodified. Returns
    // uninitialised memory (OBS: no zero-fill anywhere in the decoded layers) or nullptr.
    virtual void* allocate(std::size_t size, std::size_t align, const AllocDesc& desc) = 0;

    // ptr is non-null and was resolved to this heap by the owner map.
    virtual void deallocate(void* ptr, const AllocDesc& desc) = 0;

    // The front-end routes frees straight to the owning child instead of back through a
    // composite allocator. Wrapper allocators can use this notification to discard any
    // per-pointer bookkeeping they added during allocate().
    virtual void on_owner_map_deallocation(void*) noexcept {}

    // Called once after allocate() returned nullptr. Return true to say "handled": the front-end
    // then returns nullptr WITHOUT reporting out-of-memory. Default false (OBS: all children).
    virtual bool recover_from_failure() { return false; }
};

}  // namespace sotc
