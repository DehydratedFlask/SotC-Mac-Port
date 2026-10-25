#include "appinfo_listb.hpp"

#include <cstddef>
#include <string>
#include <utility>

namespace sotc {
namespace {

const List1bdc80& list_b_of(const Container1beeb0& container) noexcept {
    return *reinterpret_cast<const List1bdc80*>(&container.list2_prev);
}

List1bdc80& list_b_of(Container1beeb0& container) noexcept {
    return *reinterpret_cast<List1bdc80*>(&container.list2_prev);
}

// The original reads only `String.data` and re-measures with `strlen` (`0xf8f3b0`), so embedded
// NULs truncate. A null data pointer is guarded here; no observed producer passes one.
std::string holder_text(const Callable1bdeb0& holder) {
    if (holder.value_00 == 0) {
        return {};
    }
    return std::string(reinterpret_cast<const char*>(holder.value_00));
}

SharedHolderF590 release_holder(const Callable1bdeb0& holder) noexcept {
    SharedHolderF590 h{};
    h.object = reinterpret_cast<void*>(holder.value_00);
    h.control = holder.control_18;
    return h;
}

}  // namespace

AppInfoListBSerializeResult appinfo_listb_serialize(const Container1beeb0& container,
                                                    ReportItem& root) {
    AppInfoListBSerializeResult out{};
    if (root.kind() != ReportKind::kNode) {
        return out;
    }

    const List1bdc80& list = list_b_of(container);
    const auto* const sentinel = reinterpret_cast<const Node1bdc80*>(&list);
    for (const Node1bdc80* node = list.next; node != sentinel; node = node->next) {
        // 0x1be356 / 0x1be3b2: only the two data pointers are read; both are copied.
        std::unique_ptr<ReportItem> attachment = report_node_new(kAppInfoListBAttachmentType);
        std::unique_ptr<ReportItem> value = report_value_new(holder_text(node->callable_30));
        attachment->set_property(kAppInfoListBNameKey, holder_text(node->callable_10));
        if (attachment->append(std::move(value)) == nullptr) {
            return out;
        }
        if (root.append(std::move(attachment)) == nullptr) {
            return out;
        }
        ++out.entries;
        ++out.nodes;
        ++out.values;
    }

    out.ok = true;
    return out;
}

AppInfoListBClearResult appinfo_listb_clear(Container1beeb0& container,
                                            const AppInfoListBClearHooks& hooks) noexcept {
    AppInfoListBClearResult out{};
    if (hooks.free_tag2 == nullptr) {
        return out;  // visible failure: the list is untouched
    }

    List1bdc80& list = list_b_of(container);
    auto* const sentinel = reinterpret_cast<Node1bdc80*>(&list);
    Node1bdc80* node = list.next;
    while (node != sentinel) {
        Node1bdc80* const next = node->next;  // saved before the free, as at 0x1be470
        // 0x1be47b then 0x1be483: value first, name second.
        const SharedReleaseF590Result value_release =
            shared_release_f590(release_holder(node->callable_30), hooks.release);
        ++out.releases;
        if (!value_release.ok) {
            ++out.release_failures;
        }
        const SharedReleaseF590Result name_release =
            shared_release_f590(release_holder(node->callable_10), hooks.release);
        ++out.releases;
        if (!name_release.ok) {
            ++out.release_failures;
        }
        hooks.free_tag2(node);
        ++out.freed;
        node = next;
    }

    // 0x1be4a4..0x1be4b6: both links and the full 8-byte count, even for an empty list.
    list.next = sentinel;
    list.prev = sentinel;
    list.count = 0;
    out.count_after = list.count;
    out.ok = out.release_failures == 0;
    return out;
}

}  // namespace sotc
