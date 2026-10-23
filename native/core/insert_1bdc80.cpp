#include "insert_1bdc80.hpp"

namespace sotc {
namespace {

Callable1bdeb0 copy_and_acquire(const Callable1bdeb0& src, int& acquires) noexcept {
    Callable1bdeb0 temp = src;
    if (temp.control_18 != nullptr) {
        temp.control_18->fetch_add(1, std::memory_order_relaxed);
        ++acquires;
    }
    return temp;
}

bool release_again(const Callable1bdeb0& holder, const SharedReleaseF590Hooks& hooks) noexcept {
    if (holder.control_18 == nullptr) {
        return true;
    }
    SharedHolderF590 h{};
    h.object = reinterpret_cast<void*>(holder.value_00);
    h.control = holder.control_18;
    return shared_release_f590(h, hooks).ok;
}

}  // namespace

Insert1bdc80Result insert_1bdc80(Container1beeb0& container, const Callable1bdeb0& a,
                                 const Callable1bdeb0& b,
                                 const Insert1bdc80Hooks& hooks) noexcept {
    Insert1bdc80Result out{};
    if (hooks.alloc.allocate == nullptr) {
        return out;
    }

    Callable1bdeb0 temp_a = copy_and_acquire(a, out.acquires);
    Callable1bdeb0 temp_b = copy_and_acquire(b, out.acquires);
    Callable1bdeb0 temp_a2 = copy_and_acquire(temp_a, out.acquires);
    Callable1bdeb0 temp_b2 = copy_and_acquire(temp_b, out.acquires);

    const AllocWrapperResult alloc = alloc_17070(kInsert1bdc80NodeSize, hooks.alloc);
    out.allocated = alloc.ok;
    if (!alloc.ok || alloc.pointer == nullptr) {
        const bool ok_b2 = release_again(temp_b2, hooks.release);
        const bool ok_a2 = release_again(temp_a2, hooks.release);
        const bool ok_b = release_again(temp_b, hooks.release);
        const bool ok_a = release_again(temp_a, hooks.release);
        out.refs_restored = ok_b2 && ok_a2 && ok_b && ok_a;
        return out;
    }

    auto* const node = static_cast<Node1bdc80*>(alloc.pointer);
    node->callable_10 = temp_a2;  // 0x1bddb2
    node->callable_30 = temp_b2;  // 0x1bddbb

    List1bdc80& list = *reinterpret_cast<List1bdc80*>(
        reinterpret_cast<std::byte*>(&container) + kInsert1bdc80ListOffset);
    Node1bdc80* const sentinel = reinterpret_cast<Node1bdc80*>(&list);
    Node1bdc80* const old_head = list.next;
    node->prev = sentinel;       // 0x1bddc0
    node->next = old_head;       // 0x1bddce
    old_head->prev = node;       // 0x1bddd7
    list.next = node;            // 0x1bddda
    ++list.count;                // 0x1bdddf

    // The two middle releases are no-ops by construction; the last two hand the acquired refs over.
    temp_a2 = Callable1bdeb0{};
    temp_b2 = Callable1bdeb0{};
    (void)release_again(temp_a2, hooks.release);
    (void)release_again(temp_b2, hooks.release);
    (void)release_again(temp_b, hooks.release);
    (void)release_again(temp_a, hooks.release);

    out.ok = true;
    out.node = node;
    out.count_after = list.count;
    return out;
}

}  // namespace sotc
