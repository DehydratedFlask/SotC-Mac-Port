#include "addr.hpp"

#include <cstring>

namespace sotc {

std::optional<HeapWindow> HeapWindow::from_region(std::uint64_t start, std::uint64_t size) noexcept {
    const std::uint64_t limit = start + size;
    if (limit < start) return std::nullopt;
    const std::uint64_t base = start & ~std::uint64_t{Tagged31::kOffsetMask};
    if (limit - base > (std::uint64_t{1} << 31)) return std::nullopt;
    return HeapWindow{base, limit};
}

std::optional<Decoded> decode(Tagged31 v, const HeapWindow& heap) noexcept {
    const std::uint64_t off = v.offset();
    if (!v.is_heap()) {
        if (off == 0) return Decoded{Resolved::Null, 0};
        return Decoded{Resolved::Static, off};
    }
    const std::uint64_t sum = heap.base + off;
    if (sum < heap.base) return std::nullopt;                 // 64-bit overflow: the original silently wraps
    if (sum == 0) return Decoded{Resolved::Null, 0};          // original: ZF from the add => returns 0
    return Decoded{Resolved::Heap, sum};
}

bool tag_if_in_heap(Tagged31& field, const HeapWindow& heap) noexcept {
    if (field.is_heap() || field.offset() == 0) return false;
    const std::uint64_t sum = heap.base + field.offset();
    if (sum < heap.base) return false;                        // wrapped: treated as out of window (original: mod 2^64)
    if (sum >= heap.limit) return false;
    field.raw |= Tagged31::kTag;
    return true;
}

std::optional<std::uint32_t> retag_field_offset(std::uint32_t type) noexcept {
    // Original: add edi, -2; cmp edi, 0xc; ja done. Jump table cases 0..12.
    if (type < 2 || type > 14) return std::nullopt;
    switch (type) {
    case 2: return 0x0cu;
    case 3: case 4: case 5: case 6: return 0x20u;
    case 7: case 8: case 9: case 10: case 11: return std::nullopt;  // shared ret @0x66dfba
    case 12: case 14: return 0x18u;
    case 13: return 0x14u;
    default: return std::nullopt;
    }
}

bool dispatch_retag(std::uint32_t type, Tagged31 object_ref, std::uint8_t* object,
                    const HeapWindow& heap) noexcept {
    if (object == nullptr) return false;
    const auto decoded = decode(object_ref, heap);
    if (!decoded || decoded->kind == Resolved::Null) return false;  // original ZF after add -> return
    // Static/Heap both proceed in the original (any nonzero rax). Host storage is the caller's mock.
    const auto off = retag_field_offset(type);
    if (!off) return false;
    Tagged31 field{};
    std::memcpy(&field.raw, object + *off, sizeof(field.raw));
    if (!tag_if_in_heap(field, heap)) return false;
    std::memcpy(object + *off, &field.raw, sizeof(field.raw));
    return true;
}

}  // namespace sotc
