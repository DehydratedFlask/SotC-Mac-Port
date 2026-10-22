#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/host_map.hpp"

#define CHECK(c)                                                                          \
    do {                                                                                  \
        if (!(c)) {                                                                       \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);               \
            std::exit(1);                                                                 \
        }                                                                                 \
    } while (0)

int main() {
    using namespace sotc;

    unsigned char local_byte = 0;
    HostMapping zero_sized{};
    zero_sized.ptr = &local_byte;
    zero_sized.size = 0;
    CHECK(zero_sized.empty());

    HostMapping null_ptr_nonzero_size{};
    null_ptr_nonzero_size.size = 1;
    CHECK(null_ptr_nonzero_size.empty());

    // OBS rounding before ReserveVirtual (0x19e2d / 0x19f0d / 0x19f8a).
    CHECK(reserve_alignment(1) == 0x100000u);
    CHECK(reserve_alignment(0x10000) == 0x100000u);
    CHECK(reserve_alignment(0x100000) == 0x100000u);
    CHECK(reserve_alignment(0x100001u) == 0x200000u);
    CHECK(reserve_alignment(0x300000u) == 0x300000u);
    CHECK(reserve_alignment(0x20000000) == 0x20000000u);  // Shadow edx

    // Maximum size wraps during page rounding and must be rejected before mmap.
    CHECK(!map_anonymous_aligned(~std::uint64_t{0}, 0x100000).has_value());
    CHECK(!map_anonymous_aligned(0, 0x100000u).has_value());
    // A rounded 3 MiB alignment is not a power of two and is rejected before mmap.
    CHECK(!map_anonymous_aligned(0x1000u, 0x300000u).has_value());

    // Small aligned map: RW + alignment + heap_window derivation.
    {
        constexpr std::uint64_t kSize = 0x100000ull;  // 1 MiB
        auto m = map_anonymous_aligned(kSize, 0x200000u);
        CHECK(m && !m->empty());
        CHECK((reinterpret_cast<std::uintptr_t>(m->ptr) % 0x200000ull) == 0);
        CHECK(m->size >= kSize);
        auto* p = static_cast<unsigned char*>(m->ptr);
        std::memset(p, 0xA5, static_cast<size_t>(kSize));
        CHECK(p[0] == 0xA5 && p[kSize - 1] == 0xA5);

        auto view = make_region_view(reinterpret_cast<std::uint64_t>(m->ptr), kSize);
        CHECK(view);
        auto w = view->heap_window();
        CHECK(w && w->base == (view->start & ~0x7fffffffull) && w->limit == view->end());
        unmap(*m);
        CHECK(m->empty());
        CHECK(m->ptr == nullptr);
        CHECK(m->size == 0);
        CHECK(m->raw == nullptr);
        CHECK(m->raw_size == 0);
        unmap(*m);
        CHECK(m->ptr == nullptr);
        CHECK(m->size == 0);
        CHECK(m->raw == nullptr);
        CHECK(m->raw_size == 0);
    }

    // Shadow request constants with reduced size (ASan-friendly); 512 MiB align retained.
    {
        AllocatorRegionRequest shadow = kShadowHeapRequest;
        CHECK(shadow.preferred_base == 0 && shadow.size == 0x6400000ull &&
              shadow.alignment == 0x20000000u && shadow.flags == 1u);

        AllocatorRegionRequest small = shadow;
        small.size = 0x200000ull;  // 2 MiB
        auto mapped = map_region(small);
        CHECK(mapped);
        CHECK(mapped->region.size == small.size);
        CHECK(mapped->region.start == reinterpret_cast<std::uint64_t>(mapped->mapping.ptr));
        CHECK((mapped->region.start % 0x20000000ull) == 0);
        auto w = mapped->region.heap_window();
        CHECK(w && (w->base & 0x7fffffffull) == 0);
        *reinterpret_cast<std::uint32_t*>(mapped->mapping.ptr) = 0x5354434fu;
        CHECK(*reinterpret_cast<std::uint32_t*>(mapped->mapping.ptr) == 0x5354434fu);
        unmap(mapped->mapping);
    }

    {
        AllocatorRegionRequest bad = kShadowHeapRequest;
        bad.preferred_base = 0x1000;
        CHECK(!map_region(bad).has_value());
        CHECK(!map_region(AllocatorRegionRequest{}).has_value());
    }

    std::puts("host_map: all checks passed");
    return 0;
}
