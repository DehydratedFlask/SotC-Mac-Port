#include "container_dtor_1bede0.hpp"

namespace sotc {
namespace {

bool release_callable(const Callable1bdeb0& callable, const SharedReleaseF590Hooks& hooks,
                      int& releases, int& failures) noexcept {
    SharedHolderF590 holder{};
    holder.object = reinterpret_cast<void*>(callable.value_00);
    holder.control = callable.control_18;
    ++releases;
    const SharedReleaseF590Result result = shared_release_f590(holder, hooks);
    if (!result.ok) {
        ++failures;
    }
    return result.ok;
}

}  // namespace

ContainerDtor1bede0Result container_dtor_1bede0(
    Container1beeb0& container, const ContainerDtor1bede0Hooks& hooks) noexcept {
    ContainerDtor1bede0Result out{};
    if (hooks.free_tag2 == nullptr) {
        return out;  // visible failure: nothing is touched
    }

    // ---- list C (+0x40, 0x60 B callable nodes) ----
    {
        Node1beeb0* const sentinel = reinterpret_cast<Node1beeb0*>(&container.list3_prev);
        Node1beeb0* node = container.list3_prev;
        while (node != sentinel) {
            Node1beeb0* const next = node->prev;
            if (hooks.destroy_list_c_payload != nullptr) {
                hooks.destroy_list_c_payload(&node->ctx_30);
                ++out.payload_destroys;
            }
            hooks.free_tag2(node);
            ++out.freed_c;
            node = next;
        }
        container.list3_prev = sentinel;
        container.list3_next = sentinel;
        container.count3 = 0;
    }

    // ---- list B (+0x20) then list A (+0x00), 0x58 B flag nodes ----
    for (int pass = 0; pass < 2; ++pass) {
        List1bdeb0& list = pass == 0
                               ? *reinterpret_cast<List1bdeb0*>(&container.list2_prev)
                               : *reinterpret_cast<List1bdeb0*>(&container.list1_prev);
        auto* const sentinel = reinterpret_cast<Node1bdeb0*>(&list);
        Node1bdeb0* node = list.prev;
        while (node != sentinel) {
            Node1bdeb0* const next = node->prev;
            release_callable(node->callable_30, hooks.release, out.releases,
                             out.release_failures);
            release_callable(node->callable_10, hooks.release, out.releases,
                             out.release_failures);
            hooks.free_tag2(node);
            if (pass == 0) {
                ++out.freed_b;
            } else {
                ++out.freed_a;
            }
            node = next;
        }
        list.prev = sentinel;
        list.next = sentinel;
        list.count = 0;
    }

    out.ok = out.release_failures == 0;
    return out;
}

}  // namespace sotc
