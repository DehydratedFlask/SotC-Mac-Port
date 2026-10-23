#include "app_info_collect.hpp"

#include <cstddef>

namespace sotc {
namespace {

std::string_view holder_view(const Callable1bdeb0& holder) noexcept {
    if (holder.value_00 == 0) {
        return {};
    }
    return {reinterpret_cast<const char*>(holder.value_00),
            static_cast<std::size_t>(holder.value_08)};
}

// The original drains with `dec qword [sentinel+0x10]`; the List1bdeb0 view exposes the low dword
// only. Keep the 64-bit width at the byte level, as E560 already documented for the inserter.
std::uint64_t& count64(List1bdeb0& list) noexcept {
    auto* const base = reinterpret_cast<std::byte*>(&list);
    return *reinterpret_cast<std::uint64_t*>(base + kListDtor1bdeb0CountOffset);
}

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

AppInfoCollectResult collect_app_info(AppInfoRegistry& registry, AppInfoVisitor visitor, void* ctx,
                                      const AppInfoCollectHooks& hooks) noexcept {
    AppInfoCollectResult out{};
    if (visitor == nullptr || hooks.free_tag2 == nullptr) {
        return out;  // visible failure: nothing is visited or removed
    }

    List1bdeb0& list = registry.info_list();
    auto* const sentinel = reinterpret_cast<Node1bdeb0*>(&list);

    // 0x1be14d: serialize every entry. The reader does not filter on the flag.
    for (Node1bdeb0* node = list.prev; node != sentinel; node = node->prev) {
        visitor(ctx, holder_view(node->callable_10), holder_view(node->callable_30));
        ++out.visited;
    }

    // 0x1be3f4: delete every node whose dword flag is exactly 1.
    for (Node1bdeb0* node = list.prev; node != sentinel;) {
        Node1bdeb0* const next = node->prev;
        if (node->flag_50 == kAppInfoDrainFlag) {
            Node1bdeb0* const victim = next->next;  // == node; recovered as prev->next
            victim->prev->next = victim->next;
            victim->next->prev = victim->prev;
            release_callable(victim->callable_30, hooks.release, out.releases,
                             out.release_failures);
            release_callable(victim->callable_10, hooks.release, out.releases,
                             out.release_failures);
            hooks.free_tag2(victim);
            --count64(list);
            ++out.drained;
        }
        node = next;
    }

    out.count_after = list.count;
    out.ok = out.release_failures == 0;
    return out;
}

}  // namespace sotc
