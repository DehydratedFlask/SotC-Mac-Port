#include "region_list.hpp"

namespace sotc {

void list_unlink(RegionNode& node) noexcept {
    IntrusiveLinks* self = region_links(node);
    IntrusiveLinks* next = self->next;
    IntrusiveLinks* prev = self->prev;
    if (prev) prev->next = next;
    if (next) next->prev = prev;
    self->next = self;
}

void list_insert_before(RegionNode& neu, RegionNode& successor, void* owner) noexcept {
    IntrusiveLinks* self = region_links(neu);
    IntrusiveLinks* succ = region_links(successor);
    self->identity = &neu;
    self->next = succ;
    self->prev = succ->prev;
    succ->prev = self;
    if (self->prev) self->prev->next = self;
    region_owner(neu) = owner;
}

void list_insert_at_head(RegionNode& neu, IntrusiveLinks** head_next_slot, void* arena,
                         std::uint32_t* alloc_count) noexcept {
    IntrusiveLinks* self = region_links(neu);
    self->identity = &neu;
    self->prev = reinterpret_cast<IntrusiveLinks*>(arena);  // OBS: mov [r14+0x28], r15
    IntrusiveLinks* old = head_next_slot ? *head_next_slot : nullptr;
    self->next = old;
    if (head_next_slot) *head_next_slot = self;  // OBS: mov [r15+0x10], rax
    if (old) old->prev = self;
    if (alloc_count) ++(*alloc_count);
    region_owner(neu) = arena;
}

}  // namespace sotc
