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

    // OBS rounding before ReserveVirtual (0x19e2d / 0x19f0d / 0x19f8a).
    CHECK(reserve_alignment(1) == 0x100000u);
    CHECK(reserve_alignment(0x10000) == 0x100000u);
    CHECK(reserve_alignment(0x100000) == 0x100000u);
    CHECK(reserve_alignment(0x20000000) == 0x20000000u);  // Shadow edx

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
