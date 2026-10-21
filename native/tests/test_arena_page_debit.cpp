#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/arena_page_debit.hpp"

#define CHECK(c)                                                                          \
    do {                                                                                  \
        if (!(c)) {                                                                       \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);               \
            std::exit(1);                                                                 \
        }                                                                                 \
    } while (0)

int main() {
    using namespace sotc;

    alignas(8) unsigned char arena[0x80]{};
    arena_page_count(arena) = 100;
    arena_debit_pages(arena, 0x30000);  // 3 pages
    CHECK(arena_page_count(arena) == 97);
    arena_debit_pages(arena, 0xffff);  // 0 pages
    CHECK(arena_page_count(arena) == 97);
    arena_debit_pages(arena, 0x1ffff);  // one full page + a partial page
    CHECK(arena_page_count(arena) == 96);
    arena_debit_pages(nullptr, 0x10000);  // no-op

    // Idle pool insert reuses list_insert_at_head offsets.
    alignas(8) unsigned char pool[0x210]{};
    RegionNode a{}, b{};
    idle_pool_insert(a, pool);
    CHECK(idle_pool_count(pool) == 1);
    CHECK(region_owner(a) == idle_pool_sentinel(pool));
    CHECK(*idle_pool_head_slot(pool) == region_links(a));
    CHECK(region_links(a)->prev == reinterpret_cast<IntrusiveLinks*>(idle_pool_sentinel(pool)));
    CHECK(region_links(a)->next == nullptr);

    idle_pool_insert(b, pool);
    CHECK(idle_pool_count(pool) == 2);
    CHECK(*idle_pool_head_slot(pool) == region_links(b));
    CHECK(region_links(b)->next == region_links(a));
    CHECK(region_links(a)->prev == region_links(b));

    std::puts("arena_page_debit: all checks passed");
    return 0;
}
