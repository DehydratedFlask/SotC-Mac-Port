#include <cstdio>
#include <cstdlib>

#include "core/free_list_carve.hpp"

#define CHECK(c)                                                                          \
    do {                                                                                  \
        if (!(c)) {                                                                       \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);               \
            std::exit(1);                                                                 \
        }                                                                                 \
    } while (0)

int main() {
    using namespace sotc;

    // Page budget (0x1a81c): req 0x6400000 → 0x640 pages.
    CHECK(carve_pages_available(0x640, 0x6400000ull));
    CHECK(!carve_pages_available(0x63f, 0x6400000ull));
    CHECK(carve_pages_available(1, 0x10000ull));
    CHECK(!carve_pages_available(0, 0x10000ull));

    // Already-aligned free region → exact_node, whole size kept (0x1a8a1).
    {
        auto f = try_carve_fit(0x20000000ull, 0x400000ull, 0x100000ull, 0x100000u);
        CHECK(f && f->exact_node && f->lead == 0);
        CHECK(f->allocated_start == 0x20000000ull && f->allocated_size == 0x400000ull);
    }

    // Misaligned start: lead pad + remainder allocation (OBS split sizes).
    {
        // free @ 0x20001000, align 0x10000 → aligned 0x20010000, lead 0xf000
        const std::uint64_t start = 0x20001000ull;
        const std::uint64_t free_sz = 0x30000ull;
        const std::uint64_t req = 0x10000ull;
        auto f = try_carve_fit(start, free_sz, req, 0x10000u);
        CHECK(f && !f->exact_node);
        CHECK(f->lead == 0xf000ull);
        CHECK(f->allocated_start == 0x20010000ull);
        CHECK(f->allocated_size == free_sz - f->lead);  // not trimmed to req
        CHECK(f->allocated_size >= req);
    }

    // The remaining capacity exactly equals the request.
    {
        auto f = try_carve_fit(0x20001000ull, 0x1f000ull, 0x10000ull, 0x10000u);
        CHECK(f && !f->exact_node);
        CHECK(f->lead == 0xf000ull);
        CHECK(f->allocated_start == 0x20010000ull);
        CHECK(f->allocated_size == 0x10000ull);
    }

    // lead + req does not fit → nullopt (0x1a888 ja path).
    {
        auto f = try_carve_fit(0x20001000ull, 0x10000ull, 0x10000ull, 0x10000u);
        // lead=0xf000; lead+req=0x1f000 > free 0x10000
        CHECK(!f.has_value());
    }

    // free_size < req_size
    CHECK(!try_carve_fit(0x20000000ull, 0x1000ull, 0x2000ull, 0x1000u).has_value());
    // The aligned lead alone exceeds the free node, although the request fits its size.
    CHECK(!try_carve_fit(0x20001000ull, 0x1000ull, 1ull, 0x10000u).has_value());
    // Align-up addition wraps near UINT64_MAX and must reject the fit.
    CHECK(!try_carve_fit(0xfffffffffffffff8ull, 7ull, 1ull, 16u).has_value());
    // bad alignment
    CHECK(!try_carve_fit(0x20000000ull, 0x10000ull, 0x1000ull, 0u).has_value());
    CHECK(!try_carve_fit(0x20000000ull, 0x10000ull, 0x1000ull, 3u).has_value());
    CHECK(!try_carve_fit(0x20000000ull, 0x10000ull, 0ull, 0x1000u).has_value());

    // Shadow-scale numbers: 512 MiB align inside a large free region.
    {
        const std::uint64_t start = 0x21000000ull;  // 16 MiB into a 512 MiB window
        const std::uint64_t free_sz = 0x40000000ull;
        const std::uint64_t req = 0x6400000ull;
        auto f = try_carve_fit(start, free_sz, req, 0x20000000u);
        CHECK(f && !f->exact_node);
        CHECK(f->allocated_start % 0x20000000ull == 0);
        CHECK(f->lead == f->allocated_start - start);
        CHECK(f->allocated_size == free_sz - f->lead);
        CHECK(f->allocated_size >= req);
    }

    std::puts("free_list_carve: all checks passed");
    return 0;
}
