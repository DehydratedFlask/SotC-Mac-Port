#include "app_info_run.hpp"

namespace sotc {

AppInfoRunResult run_app_info_1bdf30(RegistryOwner& owner, ReportItem& root,
                                     const AppInfoSnapshotHooks& hooks) {
    AppInfoRunResult out{};
    out.dispatch = owner.dispatch();
    if (!out.dispatch.ok) {
        return out;
    }
    out.snapshot = app_info_snapshot(owner.registry(), root, hooks);
    out.ok = out.snapshot.ok;
    return out;
}

}  // namespace sotc
