// CPU-02: address classes. Three representations must never be mixed (see docs/POINTERS.md).
//   Tagged31 : the game's packed 32-bit field. bit31 = heap-relative, low 31 bits = offset (or an absolute 31-bit value).
//   Gpu40    : 40-bit GPU/descriptor address. Not a Tagged31; not dereferenceable by CPU code.
//   host pointers: never stored in 32-bit fields; this header deliberately offers no conversion from/to them.
#pragma once
#include <cstdint>
#include <optional>
#include <cstddef>

namespace sotc {

// The two runtime cells the original reads at 0x2f3b8c0 (base) and 0x2f08000 (limit): a heap window in the
// native address model. `base` is an opaque 64-bit "heap address space" origin (a handle space, not a host pointer).
struct HeapWindow {
    std::uint64_t base = 0;
    std::uint64_t limit = 0;   // exclusive upper bound compared against base + offset

    // How the original derives the cells (tools/lift/check_heap_window.py): the allocator region [start, start+size)
    // gives limit = start + size, and base = start & ~0x7fffffff (the 2 GiB-aligned window holding the region).
    // Native is stricter: nullopt when start + size overflows or the region does not fit the 31-bit offset space
    // of its window (limit - base > 2^31), cases where the original silently produces unusable tags.
    static std::optional<HeapWindow> from_region(std::uint64_t start, std::uint64_t size) noexcept;
};

struct Tagged31 {
    std::uint32_t raw = 0;

    static constexpr std::uint32_t kTag = 0x80000000u;
    static constexpr std::uint32_t kOffsetMask = 0x7fffffffu;

    constexpr bool is_heap() const noexcept { return (raw & kTag) != 0; }
    constexpr std::uint32_t offset() const noexcept { return raw & kOffsetMask; }
    // Null in the original decode = untagged zero offset (result 0). A tagged zero offset means "the heap base itself".
    constexpr bool is_null() const noexcept { return raw == 0; }
};

enum class Resolved : std::uint8_t { Null, Static, Heap };

struct Decoded {
    Resolved kind = Resolved::Null;
    std::uint64_t value = 0;   // Static: the absolute 31-bit value; Heap: base + offset in the heap address space
};

// Original 0x66dec0 prologue: p = (raw tagged ? heap.base : 0) + (raw & 0x7fffffff); zero result -> null.
// Native difference (deliberate): arithmetic overflow of base + offset is reported as nullopt instead of wrapping,
// and Static values are NOT turned into host addresses here (the image is not mapped at its console address).
std::optional<Decoded> decode(Tagged31 v, const HeapWindow& heap) noexcept;

// Original 0x66deff.. field fix-up: if untagged, nonzero, and heap.base + offset < heap.limit (unsigned 64-bit),
// set bit31. Returns true when it changed the field. Already-tagged and null fields are never touched.
bool tag_if_in_heap(Tagged31& field, const HeapWindow& heap) noexcept;

// Original 0x66dec0 type dispatcher (after decode prologue): edi=type, object at decoded esi.
// Jump table OBS @0x1270354: type' = type-2; if type' > 12 -> no-op. Field offsets:
//   type 2 -> +0x0c; types 3..6 -> +0x20; types 7..11 -> no-op; types 12,14 -> +0x18; type 13 -> +0x14.
// Returns the field byte offset to retag, or nullopt for out-of-range / nop types.
std::optional<std::uint32_t> retag_field_offset(std::uint32_t type) noexcept;

// Full 0x66dec0 behavior against host object storage: decode object_ref; if null, no-op.
// Otherwise retag the type-selected u32 field in `object` (must cover the largest offset+4).
// Returns true when a field was changed. Does not interpret Static decode as a host address.
bool dispatch_retag(std::uint32_t type, Tagged31 object_ref, std::uint8_t* object,
                    const HeapWindow& heap) noexcept;

// 40-bit GPU address: valid iff it fits in 40 bits. Distinct type so it cannot be passed where a Tagged31 is wanted.
struct Gpu40 {
    std::uint64_t value = 0;
    static constexpr std::uint64_t kMask = (std::uint64_t{1} << 40) - 1;
    static std::optional<Gpu40> make(std::uint64_t v) noexcept {
        if (v > kMask) return std::nullopt;
        return Gpu40{v};
    }
};

}  // namespace sotc
