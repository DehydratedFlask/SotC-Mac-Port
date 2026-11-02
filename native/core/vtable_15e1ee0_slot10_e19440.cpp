#include "vtable_15e1ee0_slot10_e19440.hpp"

#include <cstring>

namespace sotc {

namespace {

// Writes the adapter vtable pointer into the freshly allocated 8-byte node.
void stamp_node(void* node, std::uint64_t stamp) noexcept {
    std::memcpy(node, &stamp, sizeof(stamp));
}

}  // namespace

Vtable15e1ee0Slot10_e19440Result vtable_15e1ee0_slot10_e19440(
    const Vtable15e1ee0Slot10_e19440Cells& cells,
    const Vtable15e1ee0Slot10_e19440Hooks& hooks) noexcept {
    Vtable15e1ee0Slot10_e19440Result out{};

    AllocDispatchContract contract{};
    if (hooks.alloc != nullptr) {
        contract = *hooks.alloc;
    }

    const AllocWrapperResult a = alloc_17070(kVtable15e1ee0Slot10_e19440NodeSize, contract);
    const AllocWrapperResult b = alloc_17070(kVtable15e1ee0Slot10_e19440NodeSize, contract);
    const AllocWrapperResult c = alloc_17070(kVtable15e1ee0Slot10_e19440NodeSize, contract);
    if (!a.ok || !b.ok || !c.ok || a.pointer == nullptr || b.pointer == nullptr ||
        c.pointer == nullptr) {
        return out;  // the body stamps each node immediately, so a failed allocation cannot continue
    }
    out.node_a = a.pointer;
    out.node_b = b.pointer;
    out.node_c = c.pointer;
    stamp_node(out.node_a, kVtable15e1ee0Slot10_e19440StampA);
    stamp_node(out.node_b, kVtable15e1ee0Slot10_e19440StampB);
    stamp_node(out.node_c, kVtable15e1ee0Slot10_e19440StampC);
    out.stamped = true;
    out.allocated = true;

    if (cells.target == nullptr) {
        return out;  // `mov (%rbx),%rax` dereferences the target unconditionally after the allocs
    }
    if (hooks.dispatch_slot10 == nullptr) {
        return out;  // three slot-0x10 dispatches follow the allocations
    }

    (void)hooks.dispatch_slot10(cells.target, out.node_a);
    ++out.calls;
    (void)hooks.dispatch_slot10(cells.target, out.node_b);
    ++out.calls;
    out.code = hooks.dispatch_slot10(cells.target, out.node_c);  // tail `jmp *%rax`
    out.tail_dispatched = true;
    out.ok = true;
    return out;
}

}  // namespace sotc
