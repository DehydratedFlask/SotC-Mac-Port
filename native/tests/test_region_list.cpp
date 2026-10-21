#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/region_list.hpp"

#define CHECK(c)                                                                          \
    do {                                                                                  \
        if (!(c)) {                                                                       \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);               \
            std::exit(1);                                                                 \
        }                                                                                 \
    } while (0)

static void wire_chain(sotc::RegionNode& a, sotc::RegionNode& b, sotc::RegionNode& c) {
    using namespace sotc;
    auto* la = region_links(a);
    auto* lb = region_links(b);
    auto* lc = region_links(c);
    la->identity = &a;
    lb->identity = &b;
    lc->identity = &c;
    la->prev = nullptr;
    la->next = lb;
    lb->prev = la;
    lb->next = lc;
    lc->prev = lb;
    lc->next = nullptr;
}

int main() {
    using namespace sotc;
    CHECK(sizeof(IntrusiveLinks) == 24);
    CHECK(offsetof(IntrusiveLinks, prev) == 8 && offsetof(IntrusiveLinks, next) == 16);

    // Unlink middle of A-B-C (OBS 0x1a974..0x1a98f) + dec free_count (0x1a993).
    {
        RegionNode a{}, b{}, c{};
        wire_chain(a, b, c);
        std::uint32_t free_count = 3;
        list_unlink(b, &free_count);
        CHECK(region_links(a)->next == region_links(c));
        CHECK(region_links(c)->prev == region_links(a));
        CHECK(region_links(b)->next == region_links(b));  // self-loop
        CHECK(free_count == 2);
    }

    // Unlink the only free-list node from its sentinel.
    {
        RegionNode only{};
        IntrusiveLinks sentinel{};
        auto* link = region_links(only);
        link->identity = &only;
        sentinel.next = link;
        link->prev = &sentinel;
        link->next = nullptr;
        std::uint32_t free_count = 1;
        list_unlink(only, &free_count);
        CHECK(sentinel.next == nullptr);
        CHECK(link->next == link);
        CHECK(link->prev == &sentinel);
        CHECK(free_count == 0);
    }

    // Unlink middle of A-B-C without an optional free_count pointer.
    {
        RegionNode a{}, b{}, c{};
        wire_chain(a, b, c);
        list_unlink(b);
        CHECK(region_links(a)->next == region_links(c));
        CHECK(region_links(c)->prev == region_links(a));
        CHECK(region_links(b)->next == region_links(b));
    }

    // Unlink head of A-B-C: new head has no predecessor.
    {
        RegionNode a{}, b{}, c{};
        wire_chain(a, b, c);
        std::uint32_t free_count = 3;
        list_unlink(a, &free_count);
        CHECK(region_links(b)->prev == nullptr);
        CHECK(region_links(a)->next == region_links(a));
        CHECK(free_count == 2);
    }

    // Unlink tail of A-B-C: new tail has no successor.
    {
        RegionNode a{}, b{}, c{};
        wire_chain(a, b, c);
        std::uint32_t free_count = 3;
        list_unlink(c, &free_count);
        CHECK(region_links(b)->next == nullptr);
        CHECK(region_links(c)->next == region_links(c));
        CHECK(free_count == 2);
    }

    // Insert neu before successor (OBS 0x1a913..0x1a92f) + inc free_count (0x1a933).
    {
        RegionNode succ{}, neu{};
        auto* ls = region_links(succ);
        ls->identity = &succ;
        ls->prev = nullptr;
        ls->next = nullptr;
        void* owner = reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xDEAD));
        std::uint32_t free_count = 1;
        list_insert_before(neu, succ, owner, &free_count);
        CHECK(region_links(neu)->identity == &neu);
        CHECK(region_links(neu)->next == region_links(succ));
        CHECK(region_links(succ)->prev == region_links(neu));
        CHECK(region_owner(neu) == owner);
        CHECK(free_count == 2);
    }

    // Insert into 3-node chain before C → A-B-neu-C (nullptr free_count ok).
    {
        RegionNode a{}, b{}, c{}, neu{};
        wire_chain(a, b, c);
        list_insert_before(neu, c, nullptr);
        CHECK(region_links(b)->next == region_links(neu));
        CHECK(region_links(neu)->prev == region_links(b));
        CHECK(region_links(neu)->next == region_links(c));
        CHECK(region_links(c)->prev == region_links(neu));
    }

    // Allocated-list head insert (OBS 0x1a997..0x1a9b7).
    {
        RegionNode neu{}, already{};
        IntrusiveLinks* head = region_links(already);
        head->identity = &already;
        head->prev = nullptr;
        head->next = nullptr;
        IntrusiveLinks* slot = head;
        std::uint32_t alloc_count = 7;
        void* arena = reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xA11E));
        list_insert_at_head(neu, &slot, arena, &alloc_count);
        CHECK(slot == region_links(neu));
        CHECK(region_links(neu)->next == head);
        CHECK(head->prev == region_links(neu));
        CHECK(region_links(neu)->prev == reinterpret_cast<IntrusiveLinks*>(arena));
        CHECK(region_owner(neu) == arena);
        CHECK(alloc_count == 8);
    }

    // First allocated-list head insert starts from an empty slot.
    {
        RegionNode neu{};
        IntrusiveLinks* slot = nullptr;
        std::uint32_t alloc_count = 0;
        void* arena = reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xA11E));
        list_insert_at_head(neu, &slot, arena, &alloc_count);
        CHECK(slot == region_links(neu));
        CHECK(region_links(neu)->next == nullptr);
        CHECK(region_links(neu)->prev == reinterpret_cast<IntrusiveLinks*>(arena));
        CHECK(region_owner(neu) == arena);
        CHECK(alloc_count == 1);
    }

    // arena_free_count helper offset (OBS +0x48).
    {
        alignas(8) unsigned char buf[0x50]{};
        arena_free_count(buf) = 9;
        CHECK(*reinterpret_cast<std::uint32_t*>(buf + 0x48) == 9);
        std::uint32_t& cell = arena_free_count(buf);
        RegionNode a{}, b{}, c{};
        wire_chain(a, b, c);
        list_unlink(b, &cell);
        CHECK(cell == 8);
        list_insert_before(b, c, nullptr, &cell);
        CHECK(cell == 9);
    }

    std::puts("region_list: all checks passed");
    return 0;
}
