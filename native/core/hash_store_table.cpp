#include "hash_store_table.hpp"

#include <cstring>

#include "fnv_path_cstr_162020.hpp"

namespace sotc {

bool replay_hash_stores(std::uint8_t* image, std::size_t image_size, const HashStoreSpec& spec) noexcept {
    if (image == nullptr || spec.records == nullptr || spec.string_blob == nullptr || spec.string_offset == nullptr) return false;
    if (spec.string_blob_size == 0 || spec.string_blob[spec.string_blob_size - 1] != '\0') return false;
    for (std::size_t i = 0; i < spec.string_count; ++i) {
        const std::uint32_t off = spec.string_offset[i];
        if (off >= spec.string_blob_size) return false;
        if (std::memchr(spec.string_blob + off, '\0', spec.string_blob_size - off) == nullptr) return false;
    }
    for (std::size_t i = 0; i < spec.count; ++i) {
        const HashStoreRecord& r = spec.records[i];
        if (r.string >= spec.string_count) return false;
        if ((r.offset & 3U) != 0 || static_cast<std::size_t>(r.offset) + 4 > image_size) return false;
    }
    for (std::size_t i = 0; i < spec.count; ++i) {
        const HashStoreRecord& r = spec.records[i];
        const std::uint32_t h = fnv_cstr_a2ddc0(spec.string_blob + spec.string_offset[r.string]);
        std::memcpy(image + r.offset, &h, sizeof h);
    }
    return true;
}

}  // namespace sotc
