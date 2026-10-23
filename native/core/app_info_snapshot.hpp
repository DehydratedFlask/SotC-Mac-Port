// Native model of the `0x1bdf30` report path, phases 2-6 (E578).
//
// Byte-verified order inside the one function:
//   2. `0x1be14d..0x1be222` serialize list A into `"FieldEntry"` nodes (`0xfb3e29`), each with a
//      `"name"` property (`0xfb3e34`) and a kind-4 value child; every entry is serialized
//      regardless of its flag, text is copied.
//   4. `0x1be2f7..0x1be3e5` serialize list B into `"Attachment"` nodes (`0xfb3e4c`), same shape.
//   5. `0x1be3f4..0x1be456` drain every list-A node whose flag is exactly 1.
//   6. `0x1be458..0x1be4b6` release and free list B, then zero both links and the 8-byte count.
//
// The report copies every string, so the snapshot owns its text; the registry and list-B strings
// are released by phases 5/6 right after the copies. The native composition preserves the phase
// order (A-serialize, B-serialize, A-drain, B-clear) even where the bytes would permit another
// order.
//
// NOT modelled: the list-C dispatch before phase 2, the screenshot query `0x961a0`, the synthetic
// `"Command"` entry, the report root adoption `0xa70c30`/`0xa70d10`, and the `snapshotBugData` gate.
// G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

#include "app_info_collect.hpp"
#include "app_info_registry.hpp"
#include "appinfo_listb.hpp"
#include "report_tree.hpp"

namespace sotc {

inline constexpr std::uint64_t kAppInfoListASerializeRva = 0x1BE14D;
inline constexpr std::uint64_t kAppInfoListASerializeEndRva = 0x1BE222;
inline constexpr std::uint64_t kAppInfoListAEntryLiteralRva = 0xFB3E29;
inline constexpr const char kAppInfoListAEntryType[] = "FieldEntry";

struct AppInfoSnapshotHooks {
    AppInfoCollectHooks list_a{};
    AppInfoListBClearHooks list_b{};
    // `0x1be236..0x1be256`: append a synthetic "Command" node between the list-A and list-B
    // phases when the screenshot flag is 0. Off by default.
    bool synthetic_command = false;
};

struct AppInfoSnapshotResult {
    bool ok = false;
    int list_a_nodes = 0;   // "FieldEntry" nodes appended (phase 2)
    int list_b_nodes = 0;   // "Attachment" nodes appended (phase 4)
    int drained = 0;        // list-A flag==1 nodes removed (phase 5)
    int list_b_freed = 0;   // list-B nodes released (phase 6)
    int releases = 0;       // 0xf590 calls in phases 5 and 6
    int release_failures = 0;
    std::uint32_t list_a_count_after = 0;
};

// Runs phases 2, 4, 5 and 6 in order. `root` must be a report node. A null free hook, a value root,
// or a list-B release failure is reported through `ok`; a null free hook is checked before any
// mutation, so nothing is serialized or removed in that case. Allocation failure propagates as
// `std::bad_alloc`, matching the original throwing allocator.
[[nodiscard]] AppInfoSnapshotResult app_info_snapshot(AppInfoRegistry& registry, ReportItem& root,
                                                      const AppInfoSnapshotHooks& hooks);

}  // namespace sotc
