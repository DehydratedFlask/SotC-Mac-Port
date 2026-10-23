#include "insert_1beeb0.hpp"

namespace sotc {

Insert1beeb0Result insert_1beeb0(Container1beeb0& container,
                                 const Descriptor1beeb0& descriptor,
                                 const Insert1beeb0Alloc& alloc) noexcept {
    Insert1beeb0Result out{};

    // Step 1 — operator new(0x60). The original uses a throwing new, so a failure unwinds
    // before any list mutation. Modelled as: null alloc hook or null result => visible fail.
    if (alloc.alloc == nullptr) {
        return out;
    }
    void* raw = alloc.alloc(kInsert1beeb0NodeSize);
    if (raw == nullptr) {
        return out;
    }
    Node1beeb0* node = static_cast<Node1beeb0*>(raw);

    // Steps 2..4 — field stores, in original order.
    node->vtable = kInsert1beeb0NodeVtable;  // node[0x50] = 0x1598ce0
    node->zero_40 = 0;                       // node[0x40] = 0
    node->slot_58 = 0;                       // node[0x58] = 0 (overwritten by step 8)

    // Step 5 — 0x20 B copy descriptor[0x00..0x20] -> node[0x10..0x30]. The original is a single
    // unaligned 256-bit move (`vmovups (%r12),%ymm0` + `vmovups %ymm0,0x10(%rbx)`); modelled as
    // the four qwords it moves.
    node->obj = descriptor.ctx;
    node->invoke = descriptor.invoke;
    node->offset = descriptor.offset;
    node->fn = descriptor.fn;

    // Step 6 — self-link node[0x10] = &node[0x10] iff fn != 0 (extracted from the copied ymm0).
    const bool self_linked = (descriptor.fn != 0);
    if (self_linked) {
        node->obj = reinterpret_cast<std::uintptr_t>(&node->obj);
    }

    // Step 8 — "swap into node[0x58]": node[0x58] = descriptor[0x48]. (Steps 7 are the
    // unmodelled vcalls; they do not change the fields this model owns.)
    node->slot_58 = descriptor.slot_48;

    // Step 9 — list-3 push-front after the sentinel (&container + 0x40).
    Node1beeb0* const sentinel = reinterpret_cast<Node1beeb0*>(&container.list3_prev);
    Node1beeb0* const old_head = container.list3_next;
    node->prev = sentinel;              // node[0x00] = sentinel
    node->next = old_head;              // node[0x08] = sentinel->next (old head)
    old_head->prev = node;              // old_head[0x00] = node (or sentinel->prev when empty)
    container.list3_next = node;        // sentinel[0x08] = node

    // Step 10 — count at +0x50 (sentinel+0x10).
    ++container.count3;

    out.ok = true;
    out.node = node;
    out.copied_obj = descriptor.ctx;
    out.copied_invoke = descriptor.invoke;
    out.copied_offset = descriptor.offset;
    out.copied_fn = descriptor.fn;
    out.self_linked = self_linked;
    out.prev = node->prev;
    out.next = node->next;
    out.count = container.count3;
    return out;
}

}  // namespace sotc
