#include "app_info_snapshot.hpp"

#include <string>
#include <utility>

namespace sotc {
namespace {

std::string holder_text(const Callable1bdeb0& holder) {
    if (holder.value_00 == 0) {
        return {};
    }
    return std::string(reinterpret_cast<const char*>(holder.value_00));
}

AppInfoSnapshotResult serialize_list_a(AppInfoRegistry& registry, ReportItem& root) {
    AppInfoSnapshotResult out{};
    List1bdeb0& list = registry.info_list();
    auto* const sentinel = reinterpret_cast<Node1bdeb0*>(&list);
    for (Node1bdeb0* node = list.prev; node != sentinel; node = node->prev) {
        std::unique_ptr<ReportItem> entry = report_node_new(kAppInfoListAEntryType);
        std::unique_ptr<ReportItem> value = report_value_new(holder_text(node->callable_30));
        entry->set_property(kAppInfoListBNameKey, holder_text(node->callable_10));
        if (entry->append(std::move(value)) == nullptr) {
            return out;
        }
        if (root.append(std::move(entry)) == nullptr) {
            return out;
        }
        ++out.list_a_nodes;
    }
    out.ok = true;
    return out;
}

}  // namespace

AppInfoSnapshotResult app_info_snapshot(AppInfoRegistry& registry, ReportItem& root,
                                        const AppInfoSnapshotHooks& hooks) {
    AppInfoSnapshotResult out{};
    if (root.kind() != ReportKind::kNode || hooks.list_a.free_tag2 == nullptr ||
        hooks.list_b.free_tag2 == nullptr) {
        return out;
    }

    // Phase 2: list-A "FieldEntry" serialization.
    const AppInfoSnapshotResult serialized_a = serialize_list_a(registry, root);
    if (!serialized_a.ok) {
        return out;
    }
    out.list_a_nodes = serialized_a.list_a_nodes;

    // 0x1be236..0x1be256: synthetic "Command" entry when the screenshot flag is 0.
    if (hooks.synthetic_command) {
        std::unique_ptr<ReportItem> command = report_node_new("Command");
        std::unique_ptr<ReportItem> value = report_value_new("bug_screenshot");
        command->set_property("name", "screenshot");
        if (command->append(std::move(value)) == nullptr) {
            return out;
        }
        if (root.append(std::move(command)) == nullptr) {
            return out;
        }
    }

    // Phase 4: list-B "Attachment" serialization (before the list-A drain).
    const AppInfoListBSerializeResult serialized_b =
        appinfo_listb_serialize(registry.container(), root);
    if (!serialized_b.ok) {
        return out;
    }
    out.list_b_nodes = serialized_b.entries;

    // Phase 5: list-A flag==1 drain.
    const AppInfoDrainResult drain = drain_app_info(registry, hooks.list_a);
    out.drained = drain.drained;
    out.releases += drain.releases;
    out.release_failures += drain.release_failures;
    out.list_a_count_after = drain.count_after;

    // Phase 6: list-B release/free/reset.
    const AppInfoListBClearResult cleared = appinfo_listb_clear(registry.container(), hooks.list_b);
    out.list_b_freed = cleared.freed;
    out.releases += cleared.releases;
    out.release_failures += cleared.release_failures;

    out.ok = drain.ok && cleared.ok;
    return out;
}

}  // namespace sotc
