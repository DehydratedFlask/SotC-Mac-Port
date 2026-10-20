#include "host_map.hpp"

#include <cstdint>

#include <sys/mman.h>
#include <unistd.h>

namespace sotc {
namespace {

std::uint64_t page_size() noexcept {
    const long p = ::sysconf(_SC_PAGESIZE);
    return p > 0 ? static_cast<std::uint64_t>(p) : 16384ull;
}

bool is_pow2(std::uint64_t v) noexcept { return v != 0 && (v & (v - 1)) == 0; }

}  // namespace

std::optional<HostMapping> map_anonymous_aligned(std::uint64_t size,
                                                 std::uint32_t alignment) noexcept {
    if (size == 0) return std::nullopt;
    const std::uint32_t align = reserve_alignment(alignment);
    if (!is_pow2(align)) return std::nullopt;

    const std::uint64_t page = page_size();
    const std::uint64_t map_size = (size + page - 1) & ~(page - 1);
    if (map_size < size) return std::nullopt;
    if (map_size > ~std::uint64_t{0} - align) return std::nullopt;

    const std::uint64_t raw_size = map_size + align;
    void* raw = ::mmap(nullptr, static_cast<size_t>(raw_size), PROT_READ | PROT_WRITE,
                       MAP_ANON | MAP_PRIVATE, -1, 0);
    if (raw == MAP_FAILED) return std::nullopt;

    const auto raw_u = reinterpret_cast<std::uintptr_t>(raw);
    const std::uintptr_t aligned =
        (raw_u + static_cast<std::uintptr_t>(align) - 1) &
        ~static_cast<std::uintptr_t>(align - 1);
    const std::uint64_t lead = static_cast<std::uint64_t>(aligned - raw_u);
    const std::uint64_t trail = raw_size - lead - map_size;

    if (lead != 0) {
        if (::munmap(raw, static_cast<size_t>(lead)) != 0) {
            ::munmap(raw, static_cast<size_t>(raw_size));
            return std::nullopt;
        }
    }
    if (trail != 0) {
        void* trail_ptr = reinterpret_cast<void*>(aligned + map_size);
        if (::munmap(trail_ptr, static_cast<size_t>(trail)) != 0) {
            ::munmap(reinterpret_cast<void*>(aligned), static_cast<size_t>(map_size));
            return std::nullopt;
        }
    }

    HostMapping out;
    out.ptr = reinterpret_cast<void*>(aligned);
    out.size = map_size;
    out.raw = out.ptr;
    out.raw_size = map_size;
    auto* bytes = static_cast<unsigned char*>(out.ptr);
    bytes[0] = 0;
    bytes[map_size - 1] = 0;
    return out;
}

void unmap(HostMapping& mapping) noexcept {
    if (mapping.raw != nullptr && mapping.raw_size != 0) {
        ::munmap(mapping.raw, static_cast<size_t>(mapping.raw_size));
    }
    mapping = {};
}

std::optional<MappedRegion> map_region(const AllocatorRegionRequest& req) noexcept {
    if (req.preferred_base != 0) return std::nullopt;
    if (req.size == 0) return std::nullopt;

    auto mapped = map_anonymous_aligned(req.size, req.alignment);
    if (!mapped) return std::nullopt;

    MappedRegion out;
    out.mapping = *mapped;
    out.region.size = req.size;
    out.region.start = reinterpret_cast<std::uint64_t>(mapped->ptr);
    return out;
}

}  // namespace sotc
