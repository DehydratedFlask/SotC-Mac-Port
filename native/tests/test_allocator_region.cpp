#include <cstdio>
#include <cstdlib>

#include "core/allocator_region.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    using namespace sotc;

    // Shadow-heap request constants (OBS call site 0x55fe2c).
    CHECK(kShadowHeapRequest.preferred_base == 0);
    CHECK(kShadowHeapRequest.size == 0x6400000ull);
    CHECK(kShadowHeapRequest.alignment == 0x20000000u);
    CHECK(kShadowHeapRequest.flags == 1u);

    // Alignment helpers match the original integer updates.
    CHECK(align_up_64k(1) == 0x10000u);
    CHECK(align_up_64k(0xffff) == 0x10000u);
    CHECK(align_up_64k(0x10000) == 0x10000u);
    CHECK(align_up_64k(0x20000000) == 0x20000000u);
    CHECK(align_up_1m(1) == 0x100000u);
    CHECK(align_up_1m(0xfffff) == 0x100000u);
    CHECK(align_up_1m(0x100000) == 0x100000u);
    CHECK(align_size_up_64k(1) == 0x10000ull);
    CHECK(align_size_up_64k(0x10000) == 0x10000ull);
    CHECK(align_size_up_64k(0x6400000) == 0x6400000ull);
    CHECK(align_size_up_64k(0x10001ull) == 0x20000ull);

    // Region view → HeapWindow (same rule as HeapWindow::from_region).
    {
        auto r = make_region_view(0x2'0010'0000ull, 0x640'0000ull);
        CHECK(r && r->start == 0x2'0010'0000ull && r->size == 0x640'0000ull);
        CHECK(r->end() == r->start + r->size);
        auto w = r->heap_window();
        CHECK(w && w->base == 0x2'0000'0000ull && w->limit == r->end());
        // Layout offsets are ABI constants used by the original reader at 0x55fe4a.
        CHECK(AllocatorRegion::kSizeOffset == 0x58 && AllocatorRegion::kStartOffset == 0x60);
    }
    CHECK(!make_region_view(0, 0).has_value());
    CHECK(!make_region_view(~0ull - 8, 100).has_value());
    auto end = make_region_view(~0ull - 15, 15);
    CHECK(end && end->end() == ~0ull);

    std::puts("allocator_region: all checks passed");
    return 0;
}
