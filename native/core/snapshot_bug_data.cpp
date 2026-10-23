#include "snapshot_bug_data.hpp"

#include <utility>

namespace sotc {

SnapshotBugDataResult snapshot_bug_data(RegistryOwner& owner, ReportItem& root,
                                        const SnapshotBugDataHooks& hooks) {
    SnapshotBugDataResult out{};
    out.gate_armed = hooks.armed;
    if (!hooks.armed) {
        return out;  // nothing happens at all
    }
    out.gate_cleared = true;  // cleared before the build, even if the rest fails

    bool screenshot = false;
    if (hooks.arg2_present && hooks.query_screenshot) {
        std::vector<std::uint8_t> png;
        screenshot = hooks.query_screenshot(png);
    }

    if (screenshot) {
        const Insert1bdc80Result inserted = insert_1bdc80(
            owner.registry().container(), make_string_view_holder(kScreenshotListBName),
            make_string_view_holder(kScreenshotListBValue), hooks.list_b_insert);
        out.screenshot_attached = inserted.ok;
        if (!inserted.ok) {
            out.error = "list-b screenshot insert failed";
            return out;
        }
    }

    AppInfoSnapshotHooks snapshot = hooks.snapshot;
    snapshot.synthetic_command = !screenshot;
    const AppInfoRunResult run = run_app_info_1bdf30(owner, root, snapshot);
    out.built = run.ok;
    out.synthetic_command = !screenshot && run.ok;
    if (!run.ok) {
        out.error = "report build failed";
        return out;
    }

    if (hooks.adopt_root) {
        out.adopted = hooks.adopt_root(&root) != nullptr;
    }
    out.ok = out.built && out.adopted;
    return out;
}

}  // namespace sotc
