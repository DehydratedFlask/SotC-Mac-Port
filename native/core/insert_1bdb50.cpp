#include "insert_1bdb50.hpp"

namespace sotc {
namespace {

// 0x1bdb7c..0x1bdb9e / 0x1bdba1..0x1bdbcf: copy the caller's 0x20 B holder into a local and take
// one shared reference. The original does this before allocating, and the local is zeroed after the
// node has copied it, so the node ends up owning the single surviving reference.
Callable1bdb50 copy_and_acquire(const Callable1bdb50& src, int& acquires) noexcept {
    Callable1bdb50 temp = src;
    if (temp.control_18 != nullptr) {
        temp.control_18->fetch_add(1, std::memory_order_relaxed);  // lock incq (%rax)
        ++acquires;
    }
    return temp;
}

// Undo an acquire when the allocation failed. The original cannot reach this path (it does not
// check the allocation), so the native model has to define it: give the reference back through the
// same release the rest of the family uses, so a last reference still destroys and frees.
bool release_again(const Callable1bdb50& holder, const SharedReleaseF590Hooks& hooks) noexcept {
    if (holder.control_18 == nullptr) {
        return true;
    }
    SharedHolderF590 h{};
    h.object = reinterpret_cast<void*>(holder.value_00);
    h.control = holder.control_18;
    return shared_release_f590(h, hooks).ok;
}

}  // namespace

Insert1bdb50Result insert_1bdb50(List1bdb50& sentinel,
                                 const Callable1bdb50& a,
                                 const Callable1bdb50& b,
                                 std::uint32_t flag,
                                 const Insert1bdb50Hooks& hooks) noexcept {
    Insert1bdb50Result out{};
    if (hooks.alloc.allocate == nullptr) {
        return out;  // no allocator: nothing is touched, not even the refcounts
    }

    Callable1bdb50 temp_a = copy_and_acquire(a, out.acquires);
    Callable1bdb50 temp_b = copy_and_acquire(b, out.acquires);

    const AllocWrapperResult alloc = alloc_17070(kInsert1bdb50NodeSize, hooks.alloc);
    out.allocated = alloc.ok;
    if (!alloc.ok || alloc.pointer == nullptr) {
        const bool ok_a = release_again(temp_a, hooks.release);
        const bool ok_b = release_again(temp_b, hooks.release);
        out.refs_restored = ok_a && ok_b;
        return out;
    }

    auto* const node = static_cast<Node1bdb50*>(alloc.pointer);
    node->callable_10 = temp_a;   // 0x1bdc1e
    node->callable_30 = temp_b;   // 0x1bdc27
    node->flag_50 = flag;         // 0x1bdc2c

    auto* const sentinel_node = reinterpret_cast<Node1bdb50*>(&sentinel);
    node->prev = sentinel_node;                 // 0x1bdc30
    node->next = sentinel_node->next;           // 0x1bdc42
    sentinel_node->next->prev = node;           // 0x1bdc46
    sentinel_node->next = node;                 // 0x1bdc49

    // 0x1bdc4d: the original uses `incq [sentinel+0x10]` (64-bit, so it also bumps the 4-byte pad
    // at +0x14). The destructor decrements only the low dword (`dec dword [head+0x10]`), so the two
    // agree for any count below 2^32; the native model keeps a 32-bit count.
    ++sentinel.count;

    // 0x1bdc33..0x1bdc5b: zero both temporaries, then release them. With a null control pointer the
    // release returns immediately, which is exactly the original's effect.
    temp_b = Callable1bdb50{};
    temp_a = Callable1bdb50{};
    const bool ok_b = release_again(temp_b, hooks.release);
    const bool ok_a = release_again(temp_a, hooks.release);
    out.temp_release_ok = ok_a && ok_b;

    out.ok = true;
    out.node = node;
    out.count_after = sentinel.count;
    return out;
}

}  // namespace sotc
