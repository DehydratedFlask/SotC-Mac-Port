#include "registry_owner.hpp"

namespace sotc {

RegistryOwner::RegistryOwner(const Insert1beeb0Alloc& alloc) noexcept : alloc_(alloc) {
    // 0x256f98: the container itself is already constructed by AppInfoRegistry.
}

RegistryOwner::~RegistryOwner() {
    clear_delegates();
}

Insert1beeb0Result RegistryOwner::add_delegate(const RegistryDelegate& delegate) noexcept {
    Descriptor1beeb0 descriptor{};
    descriptor.ctx = reinterpret_cast<std::uintptr_t>(delegate.receiver);
    descriptor.invoke = reinterpret_cast<std::uintptr_t>(delegate.fn);
    descriptor.offset = 0;
    // fn == 0 keeps the insert from self-linking the receiver field; the dispatcher reads `invoke`.
    descriptor.fn = 0;
    descriptor.vtable = 0;
    descriptor.slot_48 = 0;
    return insert_1beeb0(registry_.container(), descriptor, alloc_);
}

std::uint64_t RegistryOwner::delegate_count() const noexcept {
    return registry_.container().count3;
}

RegistryOwnerDispatchResult RegistryOwner::dispatch() noexcept {
    RegistryOwnerDispatchResult out{};
    Container1beeb0& container = registry_.container();
    Node1beeb0* const sentinel = reinterpret_cast<Node1beeb0*>(&container.list3_prev);

    // 0x1bdf66: rbx = [sentinel.prev] == oldest node. 0x1bdf79: rbx = [rbx] == node.prev after
    // each call. Capture the successor first so a delegate that mutates the list cannot invalidate
    // the traversal pointer.
    Node1beeb0* node = container.list3_prev;
    while (node != sentinel) {
        Node1beeb0* const next = node->prev;
        if (node->fn != 0) {
            // Identity-only descriptor (0x1290's handler entries). Its invoke word is an original
            // RVA, which this native dispatcher must never call.
            ++out.skipped_identity;
            node = next;
            continue;
        }
        if (node->invoke == 0) {
            out.failed_node = node;
            return out;
        }
        auto* const fn = reinterpret_cast<RegistryDelegateFn>(node->invoke);
        void* const receiver =
            reinterpret_cast<void*>(node->obj + static_cast<std::uintptr_t>(node->offset));
        fn(receiver, registry_);
        ++out.called;
        node = next;
    }

    out.ok = true;
    return out;
}

void RegistryOwner::clear_delegates() noexcept {
    Container1beeb0& container = registry_.container();
    Node1beeb0* const sentinel = reinterpret_cast<Node1beeb0*>(&container.list3_prev);
    Node1beeb0* node = container.list3_next;
    while (node != sentinel) {
        Node1beeb0* const next = node->next;
        if (alloc_.dealloc_ctx != nullptr && alloc_.context != nullptr) {
            alloc_.dealloc_ctx(alloc_.context, node);
        } else if (alloc_.dealloc != nullptr) {
            alloc_.dealloc(node);
        }
        node = next;
    }
    container.list3_prev = sentinel;
    container.list3_next = sentinel;
    container.count3 = 0;
}

}  // namespace sotc
