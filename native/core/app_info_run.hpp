// Native composition of the measured `0x1bdf30` path: list-C dispatch, then phases 2/4/5/6 (E584).
//
// `0x1bdf30(container, ...)` starts with the list-C delegate walk (`0x1bdf66` loads
// `container+0x40`, `0x1bdf79` follows `node+0x00`) and then serializes list A, serializes list B,
// drains list-A flag==1 and clears list B. The native pieces already exist:
//   - `RegistryOwner::dispatch()` models only the list-C walk;
//   - `app_info_snapshot()` models phases 2/4/5/6.
// This module runs them in the recovered order. It is still not the full function: the
// `snapshotBugData` gate, the screenshot query and root adoption are caller contracts.
//
// A dispatch failure stops before any serialization, matching the native visible-failure policy.
// G1 PARTIAL.
#pragma once

#include "app_info_snapshot.hpp"
#include "registry_owner.hpp"

namespace sotc {

inline constexpr std::uint64_t kAppInfoRunDispatchStartRva = 0x1BDF66;
inline constexpr std::uint64_t kAppInfoRunDispatchNextRva = 0x1BDF79;

struct AppInfoRunResult {
    bool ok = false;
    RegistryOwnerDispatchResult dispatch{};
    AppInfoSnapshotResult snapshot{};
};

// Dispatches the owner's list-C delegates, then snapshots the registry into `root`.
[[nodiscard]] AppInfoRunResult run_app_info_1bdf30(RegistryOwner& owner, ReportItem& root,
                                                   const AppInfoSnapshotHooks& hooks);

}  // namespace sotc
