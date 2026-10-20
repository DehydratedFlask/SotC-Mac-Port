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

    // Unlink middle of A-B-C (OBS 0x1a974..0x1a98f).
    {
        RegionNode a{}, b{}, c{};
        wire_chain(a, b, c);
        list_unlink(b);
        CHECK(region_links(a)->next == region_links(c));
        CHECK(region_links(c)->prev == region_links(a));
        CHECK(region_links(b)->next == region_links(b));  // self-loop
    }

    // Insert neu before successor (OBS 0x1a913..0x1a92f).
    {
        RegionNode succ{}, neu{};
        auto* ls = region_links(succ);
        ls->identity = &succ;
        ls->prev = nullptr;
        ls->next = nullptr;
        void* owner = reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xDEAD));
        list_insert_before(neu, succ, owner);
        CHECK(region_links(neu)->identity == &neu);
        CHECK(region_links(neu)->next == region_links(succ));
        CHECK(region_links(succ)->prev == region_links(neu));
        CHECK(region_owner(neu) == owner);
    }

    // Insert into 3-node chain before C → A-B-neu-C.
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

    std::puts("region_list: all checks passed");
    return 0;
}
