#include "store_table.hpp"

#include <cstring>

namespace sotc {

namespace {

[[nodiscard]] bool lane_ok(std::uint8_t kind, std::uint64_t value, std::uint8_t width, std::size_t image_size, const StoreTableSpec& spec) noexcept {
    switch (static_cast<StoreLaneKind>(kind)) {
        case StoreLaneKind::constant:
            return width != 4 || value <= 0xffffffffull;
        case StoreLaneKind::self:
            return width >= 8 && value <= image_size && image_size - value >= 8;   // the pointee must be inside the image
        case StoreLaneKind::string:
            return width >= 8 && value < spec.string_count && spec.string_offset[value] < spec.string_blob_size;
        case StoreLaneKind::tag:
            return width >= 8;
    }
    return false;
}

[[nodiscard]] std::uint64_t lane_value(std::uint8_t kind, std::uint64_t value, std::uint8_t* image, const StoreTableSpec& spec) noexcept {
    switch (static_cast<StoreLaneKind>(kind)) {
        case StoreLaneKind::self:
            return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(image + value));
        case StoreLaneKind::string:
            return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(spec.string_blob + spec.string_offset[value]));
        case StoreLaneKind::constant:
        case StoreLaneKind::tag:
            break;
    }
    return value;
}

}  // namespace

bool replay_store_table(std::uint8_t* image, std::size_t image_size, const StoreTableSpec& spec) noexcept {
    if (image == nullptr || spec.records == nullptr || spec.count == 0) return false;
    if (spec.string_count != 0 && (spec.string_blob == nullptr || spec.string_offset == nullptr || spec.string_blob_size == 0 || spec.string_blob[spec.string_blob_size - 1] != '\0')) return false;   // every string is terminated
    // pass 1: validate everything; a refused table leaves the image untouched
    for (std::size_t i = 0; i < spec.count; ++i) {
        const StoreTableRecord& r = spec.records[i];
        if (r.reserved != 0) return false;
        if (r.width != 4 && r.width != 8 && r.width != 16) return false;
        if (r.offset > image_size || image_size - r.offset < r.width) return false;
        if (!lane_ok(r.kind0, r.value0, r.width, image_size, spec)) return false;
        if (r.width == 16) {
            if (!lane_ok(r.kind1, r.value1, r.width, image_size, spec)) return false;
        } else if (r.kind1 != 0 || r.value1 != 0) {
            return false;
        }
    }
    // pass 2: apply in program order
    for (std::size_t i = 0; i < spec.count; ++i) {
        const StoreTableRecord& r = spec.records[i];
        const std::uint64_t v0 = lane_value(r.kind0, r.value0, image, spec);
        if (r.width == 4) {
            const std::uint32_t d = static_cast<std::uint32_t>(v0);
            std::memcpy(image + r.offset, &d, 4);
        } else {
            std::memcpy(image + r.offset, &v0, 8);
            if (r.width == 16) {
                const std::uint64_t v1 = lane_value(r.kind1, r.value1, image, spec);
                std::memcpy(image + r.offset + 8, &v1, 8);
            }
        }
    }
    return true;
}

}  // namespace sotc
