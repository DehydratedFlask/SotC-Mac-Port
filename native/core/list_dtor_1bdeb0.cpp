#include "list_dtor_1bdeb0.hpp"

namespace sotc {
namespace {
SharedHolderF590 as_holder(const Callable1bdeb0& c) noexcept {
    SharedHolderF590 h{};
    h.object = reinterpret_cast<void*>(c.value_00);
    h.control = c.control_18;
    return h;
}
}  // namespace

ListDtor1bdeb0Result list_dtor_1bdeb0(List1bdeb0& list,
                                      const ListDtor1bdeb0Hooks& hooks) noexcept {
    ListDtor1bdeb0Result out{};
    if (hooks.free_tag2 == nullptr) {
        return out;  // cannot free: report failure without touching the list
    }

    Node1bdeb0* const sentinel = reinterpret_cast<Node1bdeb0*>(&list.prev);
    Node1bdeb0* node = list.prev;  // [head]
    while (node != sentinel && node != nullptr) {
        if (node->flag_50 != 0) {
            node = node->prev;  // skip flagged nodes
            ++out.skipped;
            continue;
        }
        node = node->prev;                       // advance first (1bdedf)
        Node1bdeb0* const victim = node->next;   // == the node we just left (1bdee2)

        // Unlink (1bdee6..1bdef9).
        victim->prev->next = victim->next;
        victim->next->prev = victim->prev;

        // Release +0x30 then +0x10 through 0xf590 (1bdefc / 1bdf05).
        const SharedReleaseF590Result r30 = shared_release_f590(as_holder(victim->callable_30),
                                                                hooks.release);
        const SharedReleaseF590Result r10 = shared_release_f590(as_holder(victim->callable_10),
                                                                hooks.release);
        out.releases += 2;
        if (!r30.ok || !r10.ok) {
            return out;  // a release could not be completed: do not pretend the node was freed
        }

        // Tag-2 free (1bdf12).
        hooks.free_tag2(static_cast<void*>(victim));
        ++out.frees;

        // dec dword [head+0x10] (1bdf17).
        if (list.count != 0) --list.count;
        ++out.erased;
        out.count_after = list.count;
    }

    out.ok = true;
    out.count_after = list.count;
    return out;
}

}  // namespace sotc
