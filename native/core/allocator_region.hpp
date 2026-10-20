// CPU-02: allocator region API contract for original RVA 0x19ae0 (and inner writer at 0x19e00).
// This is a typed contract + layout, not a lift of the full allocator body (spinlock, page tables,
// sceKernel* mapping). Those remain SRV-01. See docs/POINTERS.md and tools/lift/check_f19ae0.py.
#pragma once
#include <cstdint>
#include <optional>

#include "addr.hpp"

namespace sotc {

// Returned object R from 0x19ae0. Only the fields proven by the Shadow-heap call site and the
// store sequence at 0x19fd9..0x19fe1 are named here. Other bytes are opaque.
struct AllocatorRegion {
    static constexpr std::size_t kSizeOffset = 0x58;   // u64 byte length written at 0x19fd9
    static constexpr std::size_t kStartOffset = 0x60;  // u64 start written at 0x19fdd
    static constexpr std::size_t kFlagAOffset = 0x68;  // u8 copied at 0x19fd6
    static constexpr std::size_t kFlagBOffset = 0x69;  // u8 set to 1 at 0x19fe1

    std::uint64_t size = 0;
    std::uint64_t start = 0;

    std::uint64_t end() const noexcept { return start + size; }

    // Same derivation the game uses after the call (check_heap_window.py).
    std::optional<HeapWindow> heap_window() const noexcept {
        return HeapWindow::from_region(start, size);
    }
};

// SysV args to 0x19ae0 as observed at 0x55fe2c and forwarded into 0x19e00 at 0x19b68..0x19b71.
// Labels: size/align/flags roles are OBS from use; preferred_base meaning is INF (often 0).
struct AllocatorRegionRequest {
    std::uint64_t preferred_base = 0;  // rdi — 0 at the Shadow-heap site; object ptr at 0x479a0 path
    std::uint64_t size = 0;            // rsi — becomes R[+0x58]
    std::uint32_t alignment = 0;       // edx — rounded up to 64 KiB then (on some paths) 1 MiB
    std::uint32_t flags = 0;           // ecx — stack arg to 0x19e00; value 1 selects the fast path
};

// Observed at 0x55fe2c for the main game heap region.
inline constexpr AllocatorRegionRequest kShadowHeapRequest{
    .preferred_base = 0,
    .size = 0x6400000ull,      // 100 MiB
    .alignment = 0x20000000u,  // 512 MiB
    .flags = 1u,
};

// Original 0x19e00 prologue rounding of the alignment argument (edx → ebx):
//   add 0xffff; and 0xffff0000  → ceil to 64 KiB.
inline std::uint32_t align_up_64k(std::uint32_t alignment) noexcept {
    return (alignment + 0xffffu) & 0xffff0000u;
}

// Later path at 0x19f0d: add 0xfffff; and 0xfff00000 → ceil to 1 MiB.
inline std::uint32_t align_up_1m(std::uint32_t alignment) noexcept {
    return (alignment + 0xfffffu) & 0xfff00000u;
}

// 0x479a0 size prep before calling 0x19ae0: add 0xffff; and -0x10000 → ceil to 64 KiB.
inline std::uint64_t align_size_up_64k(std::uint64_t size) noexcept {
    return (size + 0xffffull) & ~0xffffull;
}

// Native stand-in: build a region view from an already-mapped [start, start+size) without
// calling the original allocator. Fails visibly (nullopt) on overflow / zero size.
std::optional<AllocatorRegion> make_region_view(std::uint64_t start, std::uint64_t size) noexcept;

}  // namespace sotc
