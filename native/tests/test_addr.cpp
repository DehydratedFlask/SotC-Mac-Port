// Boundary / invalid / alias tests against the specification derived from the observed code (docs/POINTERS.md).
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

#include "core/addr.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)
using namespace sotc;

int main() {
    const HeapWindow heap{0x1'0000'0000ull, 0x1'0000'0000ull + 0x1000};   // 4 KiB window, arbitrary test origin

    // Null / static / heap classification.
    CHECK(decode({0}, heap)->kind == Resolved::Null);
    CHECK(decode({0x00401234}, heap)->kind == Resolved::Static && decode({0x00401234}, heap)->value == 0x401234);
    auto h = decode({0x80000010}, heap);
    CHECK(h && h->kind == Resolved::Heap && h->value == heap.base + 0x10);
    CHECK(decode({0x80000000}, heap)->value == heap.base);                  // tagged zero offset = heap base, not null

    // Width boundaries: 31-bit offset max; the tag bit never leaks into the offset.
    CHECK(Tagged31{0x7fffffff}.offset() == 0x7fffffffu && !Tagged31{0x7fffffff}.is_heap());
    CHECK(Tagged31{0xffffffff}.offset() == 0x7fffffffu && Tagged31{0xffffffff}.is_heap());
    CHECK(decode({0xffffffff}, heap)->value == heap.base + 0x7fffffff);

    // Overflow: base near 2^64 is rejected rather than wrapped.
    const HeapWindow top{std::numeric_limits<std::uint64_t>::max() - 5, std::numeric_limits<std::uint64_t>::max()};
    CHECK(!decode({0x80000010}, top).has_value());
    CHECK(decode({0x80000005}, top).has_value());                           // exactly max(): no overflow
    // Zero base: tagged zero offset sums to 0 -> null, as the original's ZF test does.
    CHECK(decode({0x80000000}, HeapWindow{0, 0x1000})->kind == Resolved::Null);

    // Retag rule: only untagged, nonzero, in-window values change.
    Tagged31 f{0x00000800};
    CHECK(tag_if_in_heap(f, heap) && f.raw == 0x80000800);
    CHECK(!tag_if_in_heap(f, heap) && f.raw == 0x80000800);                 // already tagged: untouched, idempotent
    Tagged31 edge_in{0x00000fff}, edge_out{0x00001000}, zero{0};
    CHECK(tag_if_in_heap(edge_in, heap) && edge_in.raw == 0x80000fff);      // base+0xfff < limit
    CHECK(!tag_if_in_heap(edge_out, heap) && edge_out.raw == 0x00001000);   // base+0x1000 == limit: not < limit
    CHECK(!tag_if_in_heap(zero, heap) && zero.raw == 0);
    // Aliasing: an untagged value and the tagged form of the same offset decode to different classes.
    CHECK(decode({0x00000800}, heap)->kind == Resolved::Static && decode({0x80000800}, heap)->kind == Resolved::Heap);
    // A static absolute value above the window is left alone (would be a host-image address in the original).
    Tagged31 stat{0x00401234};
    CHECK(!tag_if_in_heap(stat, heap) && stat.raw == 0x00401234);
    // Wrap-around window: base+offset wraps -> never tagged.
    Tagged31 w{0x7fffffff};
    CHECK(!tag_if_in_heap(w, HeapWindow{std::numeric_limits<std::uint64_t>::max() - 5, 0xffff}));

    // Window derivation from an allocator region (observed rule: base = start & ~0x7fffffff, limit = start + size).
    {
        const std::uint64_t start = 0x2'0010'0000ull, size = 0x640'0000ull;      // 100 MiB inside window 0x2'0000'0000
        auto w = HeapWindow::from_region(start, size);
        CHECK(w && w->base == 0x2'0000'0000ull && w->limit == start + size);
        // One-sided check (observed): offsets BELOW the region start but inside the 2 GiB window are still tagged.
        Tagged31 below{0x00000800};
        CHECK(tag_if_in_heap(below, *w) && below.raw == 0x80000800);
        Tagged31 at_end{static_cast<std::uint32_t>(w->limit - w->base)};
        CHECK(!tag_if_in_heap(at_end, *w));                                       // exactly limit: excluded
        Tagged31 last{static_cast<std::uint32_t>(w->limit - w->base - 1)};
        CHECK(tag_if_in_heap(last, *w) && decode(last, *w)->value == w->limit - 1);
        // Round trip: tag then decode returns base + offset.
        Tagged31 rt{0x00100123};
        CHECK(tag_if_in_heap(rt, *w) && decode(rt, *w)->value == w->base + 0x100123);
    }
    CHECK(!HeapWindow::from_region(~0ull - 10, 100).has_value());                 // start + size overflows
    CHECK(!HeapWindow::from_region(0x2'7000'0000ull, 0x2000'0000ull).has_value()); // 512 MiB region straddles the 2 GiB window end
    CHECK(HeapWindow::from_region(0x2'0000'0000ull, 0x8000'0000ull).has_value());   // exactly 2 GiB fits
    CHECK(!HeapWindow::from_region(0x2'0000'0000ull, 0x8000'0001ull).has_value());

    // 40-bit GPU addresses.
    CHECK(Gpu40::make(0).has_value() && Gpu40::make((1ull << 40) - 1).has_value());
    CHECK(!Gpu40::make(1ull << 40).has_value() && !Gpu40::make(~0ull).has_value());
    static_assert(sizeof(Tagged31) == 4 && sizeof(Gpu40) == 8);

    // 0x66dec0 type dispatcher: jump-table field offsets and nop types.
    CHECK(retag_field_offset(2) && *retag_field_offset(2) == 0x0c);
    CHECK(retag_field_offset(3) && *retag_field_offset(3) == 0x20);
    CHECK(retag_field_offset(6) && *retag_field_offset(6) == 0x20);
    CHECK(!retag_field_offset(7) && !retag_field_offset(11));           // nop arms
    CHECK(retag_field_offset(12) && *retag_field_offset(12) == 0x18);
    CHECK(retag_field_offset(13) && *retag_field_offset(13) == 0x14);
    CHECK(retag_field_offset(14) && *retag_field_offset(14) == 0x18);
    CHECK(!retag_field_offset(0) && !retag_field_offset(1) && !retag_field_offset(15));

    {
        alignas(4) std::uint8_t obj[0x28]{};
        // Seed untagged in-window values at every retag offset.
        const std::uint32_t seed = 0x00000800;
        std::memcpy(obj + 0x0c, &seed, 4);
        std::memcpy(obj + 0x14, &seed, 4);
        std::memcpy(obj + 0x18, &seed, 4);
        std::memcpy(obj + 0x20, &seed, 4);
        const Tagged31 obj_ref{0x80000100};  // nonzero heap decode => dispatch proceeds
        CHECK(dispatch_retag(2, obj_ref, obj, heap));
        std::uint32_t v = 0;
        std::memcpy(&v, obj + 0x0c, 4);
        CHECK(v == 0x80000800);
        CHECK(dispatch_retag(4, obj_ref, obj, heap));                   // type 4 -> +0x20
        std::memcpy(&v, obj + 0x20, 4);
        CHECK(v == 0x80000800);
        CHECK(!dispatch_retag(8, obj_ref, obj, heap));                  // nop type
        CHECK(!dispatch_retag(2, Tagged31{0}, obj, heap));              // null object_ref
        CHECK(dispatch_retag(13, obj_ref, obj, heap));
        std::memcpy(&v, obj + 0x14, 4);
        CHECK(v == 0x80000800);
    }

    std::puts("addr: all checks passed");
    return 0;
}
